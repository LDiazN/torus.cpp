#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <assert.h>
#include <cmath>

using namespace std;

const int RESOLUTION = 20;
const int TORUS_RESOLUTION = 360;
const float EYE_DISTANCE_TO_SCREEN = 20;

void clear_canvas(char (&canvas)[RESOLUTION][RESOLUTION])
{
    for(int i = 0; i < RESOLUTION; i++)
        for(int j = 0; j < RESOLUTION; j++)
            canvas[i][j] = ' ';
}

void cross(float x1, float y1, float z1, float x2, float y2, float z2, float& out_x, float& out_y, float& out_z)
{
    out_x = y1 * z2 - z1 * y2;
    out_y = z1 * x1 - x1 * z2;
    out_z = x1 * y2 - y1 * x2;
}

float dot(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

void normalize(float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    const float magnitude = sqrt(x*x + y*y + z*z);
    out_x = x / magnitude;
    out_y = y / magnitude;
    out_z = z / magnitude;
}

void torus_point(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    assert(tube_radius < torus_radius && "Radius of tube should be smaller than radius of torus");
    out_x = (torus_radius + tube_radius * cos(theta)) * cos(phi);
    out_y = (torus_radius + tube_radius * cos(theta)) * sin(phi);
    out_z = -tube_radius * sin(theta);
}

void torus_normal(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    // We will compute a point in the middle of the tube and use it to compute the direction of the normal using a point in the torus surface
    float surface_x, surface_y, surface_z;
    torus_point(theta, phi, torus_radius, tube_radius, surface_x, surface_y, surface_z);
    float middle_x, middle_y, middle_z;
    middle_x = (torus_radius) * cos(phi);
    middle_y = (torus_radius) * sin(phi);
    middle_z = 0;

    out_x = (surface_x - middle_x);
    out_y = (surface_y - middle_y);
    out_z = (surface_z - middle_z);

    // Note that the vector will have magnitude == tube_radius, this can be simplified by dividing each coordinate 
    // by tube_radius
    normalize(out_x, out_y, out_z, out_x, out_y, out_z);
}

void translate(float offset_x, float offset_y, float offset_z, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x + offset_x;
    out_y = y + offset_y;
    out_z = z + offset_z;
}

// Rotate a point around the X axis
void rotate_x(float angle, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x;
    out_y = y * cos(angle) - z * sin(angle);
    out_z = y * sin(angle) + z * cos(angle);
}

// Rotate a point around the Y axis
void rotate_y(float angle, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x * cos(angle) + z * sin(angle);
    out_y = y;
    out_z = -x * sin(angle) + z * cos(angle);
}

void rotate_z(float angle, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x * cos(angle) + y * sin(angle);
    out_y = -x * sin(angle) + y * cos(angle);
    out_z = z;
}

void update_canvas(char (&canvas)[RESOLUTION][RESOLUTION], float time_passed)
{

    // Define light direction for illumination
    float LIGHT_DIR_X = 0, LIGHT_DIR_Y = 1, LIGHT_DIR_Z = 1;
    normalize(LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z, LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z);

    // Define shade table: used to find the right color for this pixel. We have 12 shades.
    char const SHADES[] = ".,-~:;=!*#$@";


    // Clear canvas so the previous image is removed
    clear_canvas(canvas);
    
    // Clear depth buffer. We use it to check whether we should write a new pixel depending on how far
    // the current pixel is currently from the camera
    float depth_buffer[RESOLUTION][RESOLUTION];
    for(int i = 0; i < RESOLUTION; i++)
        for(int j = 0; j < RESOLUTION; j++)
            depth_buffer[i][j] = 1000000;

    // To generate torus points we have to define a step size that 
    // will be the offset between angles we have to pass to the torus function.
    const float step_size = (2.f * M_PI) / TORUS_RESOLUTION;
    for (int i = 0; i < TORUS_RESOLUTION; i++)
    {
        const float phi = step_size * i;
        for (int j = 0; j < TORUS_RESOLUTION; j++)
        {
            const float theta = step_size * j;

            // phi: Torus radius
            // theta: Tube radius

            float x, y, z; // point coordinates
            float n_x, n_y, n_z; // point normal

            // Compute torus surface point and normal in that same point
            const float torus_radius = ((float) RESOLUTION) / 2.f;
            const float tube_radius = ((float) RESOLUTION) / 4.f;
            torus_point(theta, phi,torus_radius, tube_radius, x, y, z);
            torus_normal(theta, phi, torus_radius, tube_radius, n_x, n_y, n_z);

            // transform this point
            rotate_y(time_passed, x,y,z, x,y,z);
            rotate_x(time_passed * 1.13, x,y,z, x,y,z);
            rotate_z(time_passed * 1.74, x,y,z, x,y,z);
            translate((torus_radius + tube_radius) * 1.20, (torus_radius + tube_radius) * 1.20, 40, x,y,z, x,y,z);

            // transform normal: note that we only about rotation since translation doesn't apply and scale changes the size of the normal vector 
            // which must reamin of unit size
            rotate_y(time_passed, n_x,n_y,n_z, n_x,n_y,n_z);
            rotate_x(time_passed * 1.13, n_x,n_y,n_z, n_x,n_y,n_z);
            rotate_z(time_passed * 1.74, n_x,n_y,n_z, n_x,n_y,n_z);

            // Project this point to the screen: 
            int x_int, y_int;
            x_int = (int) ((EYE_DISTANCE_TO_SCREEN * x / z));
            y_int = (int) ((EYE_DISTANCE_TO_SCREEN * y / z));

            // Use depth buffer to check if this pixel should be shaded. Also check if the point is within the limits
            // of the screen
            if (0 <= x_int && x_int < RESOLUTION && 0 <= y_int && y_int < RESOLUTION && depth_buffer[y_int][x_int] > z)
            {
                // Shade this pixel
                float normal_dot_light = dot(n_x,n_y,n_z, LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z);

                // Since normal vector and light direction vector are both unit sized, then the dot is in the range [-1, 1].
                // we want to avoid the limits of this range to prevent out of range errors
                if (abs(normal_dot_light) >= 1.0f)
                    normal_dot_light = normal_dot_light * 0.99;

                // Shade according to lighting: If normal and light are paralel, then this point is facing away of light.
                // Otherwise, it's facing the light and we want to choose a shade based on how much antiparallel it is relative 
                // to light direction

                if(normal_dot_light > 0)
                    canvas[y_int][x_int] = ' ';
                else 
                    canvas[y_int][x_int] = SHADES[(int) (abs(normal_dot_light) * 12)];

                depth_buffer[y_int][x_int] = z;
            }
        }
    }

}

int main() {

    // Where we're gonna draw
    char canvas[RESOLUTION][RESOLUTION];
    
    // Initialization
    clear_canvas(canvas);

    // Main loop
    auto frame_start = chrono::high_resolution_clock::now();
    float time_passed = 0;
    while (true)
    {
        // Compute delta time: the time between the last frame start and now
        auto now = chrono::high_resolution_clock::now();
        auto delta_time = chrono::duration_cast<chrono::microseconds>(now - frame_start).count() / 1000000.0f;
        time_passed += delta_time;

        frame_start = now;

        // Update canvas based on delta time
        update_canvas(canvas, time_passed);

        // print canvas to terminal
        for (int i = 0; i < RESOLUTION; i++)
        {
            for (int j = 0; j < RESOLUTION; j++)
            {
                cout << canvas[i][j] << " ";
            }
            cout << endl;
        }

        // This is equivalent to a swap buffers
        cout << flush;

        // Wait for next update
        this_thread::sleep_for(chrono::milliseconds(30));

        // Reset cursor back to draw position
        cout << "\033[" << RESOLUTION << "A";
    }
    
    return 0;
}