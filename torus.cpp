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

void torus(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    assert(tube_radius < torus_radius && "Radius of tube should be smaller than radius of torus");
    out_x = (torus_radius + tube_radius * cos(theta)) * cos(phi);
    out_y = (torus_radius + tube_radius * cos(theta)) * sin(phi);
    out_z = -tube_radius * sin(theta);
}

void torus_tangent(float theta, float phi, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    out_x = -tube_radius * sin(theta) * cos(phi);
    out_y = -tube_radius * sin(theta) * sin(phi);
    out_z = -tube_radius * cos(theta);
}

void torus_bitangent(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    out_x = -(torus_radius + tube_radius * cos(theta)) * sin((phi));
    out_y = -(torus_radius + tube_radius * cos(theta)) * cos(phi);
    out_z = 0;
}

void torus_normal(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    float tangent_x, tangent_y, tangent_z;
    float bitangent_x, bitangent_y, bitangent_z;
    torus_tangent(theta, phi, tube_radius, tangent_x, tangent_y, tangent_z);
    torus_bitangent(theta, phi, torus_radius, tube_radius, bitangent_x, bitangent_y, bitangent_z);

    cross(tangent_x, tangent_y, tangent_z, bitangent_x, bitangent_y, bitangent_z, out_x, out_y, out_z);
    normalize(out_x, out_y, out_z, out_x, out_y, out_z);

    // DEBUG ONLY
    // out_x = cos(theta);
    // out_y = sin(theta);
    // out_z = 0;
}

void scale(float scale, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = scale * x;
    out_y = scale * y;
    out_z = scale * z;
}

void translate(float offset_x, float offset_y, float offset_z, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x + offset_x;
    out_y = y + offset_y;
    out_z = z + offset_z;
}

// Rotate a point around the Y axis
void rotate_y(float angle, float x, float y, float z, float& out_x, float& out_y, float& out_z)
{
    out_x = x * cos(angle) + z * sin(angle);
    out_y = y;
    out_z = -x * sin(angle) + z * cos(angle);
}

void update_canvas(char (&canvas)[RESOLUTION][RESOLUTION], float time_passed)
{
    assert(RESOLUTION > 10);
    float depth_buffer[RESOLUTION][RESOLUTION];

    // Define light direction for illumination
    float LIGHT_DIR_X = 0, LIGHT_DIR_Y = 0, LIGHT_DIR_Z = 1;
    normalize(LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z, LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z);

    // Define shade table: used to find the right color for this pixel. We have 12 shades.
    char const SHADES[] = ".,-~:;=!*#$@";


    // Clear canvas so the previous image is removed
    clear_canvas(canvas);
    
    // Clear depth buffer. We store Z^-1 instead of Z, so 0 represents infinite distance
    for(int i = 0; i < RESOLUTION; i++)
        for(int j = 0; j < RESOLUTION; j++)
            depth_buffer[i][j] = 1000000;


    // Now we will compute all the points in the torus based on some resolution, and
    // then we will transform them properly.

    // To generate torus points we have to define a step size that 
    // will be the offset between angles we have to pass to the torus function.
    const float step_size = (2.f * M_PI) / TORUS_RESOLUTION;
    for (int i = 0; i < TORUS_RESOLUTION; i++)
    {
        const float phi = step_size * i;
        for (int j = 0; j < TORUS_RESOLUTION; j++)
        {
            const float theta = step_size * j;
            float x, y, z; // point coordinates
            float n_x, n_y, n_z; // point normal

            // Compute this point
            const float torus_radius = ((float) RESOLUTION) / 2.f;
            const float tube_radius = ((float) RESOLUTION) / 4.f;
            torus(theta, phi,torus_radius, tube_radius, x, y, z);
            torus_normal(theta, phi, torus_radius, tube_radius, n_x, n_y, n_z);

            n_x += cos(phi);
            n_y += sin(phi);
            n_z += 0;

            // Value stored in Depth Buffer
            const float z_inv = z != 0 ? 1 / z : 0;

            // transform this point
            rotate_y(time_passed, x,y,z, x,y,z);
            //scale(0.3f, x,y,z, x,y,z);
            translate(torus_radius + tube_radius, torus_radius + tube_radius, 40, x,y,z, x,y,z);

            // transform normal: note that we only about rotation since translation doesn't apply and scale changes the size of the normal vector 
            // which must reamin of unit size
            rotate_y(time_passed, n_x,n_y,n_z, n_x,n_y,n_z);

            // Project this point to the screen: 
            int x_int, y_int;
            x_int = (int) ((EYE_DISTANCE_TO_SCREEN * x / z));
            y_int = (int) ((EYE_DISTANCE_TO_SCREEN * y / z));

            if (0 <= x_int && x_int < RESOLUTION && 0 <= y_int && y_int < RESOLUTION && depth_buffer[y_int][x_int] > z)
            {
                // Choose shade of pixel 
                float normal_dot_light = dot(n_x,n_y,n_z, LIGHT_DIR_X, LIGHT_DIR_Y, LIGHT_DIR_Z);

                // Since normal vector and light direction vector are both unit sized, then the dot is in the range [-1, 1].
                // we want to avoid the limits of this range to prevent out of range errors
                if (abs(normal_dot_light) >= 1.0f)
                    normal_dot_light = normal_dot_light * 0.99;

                // Choose lighting:
                if(normal_dot_light > 0 || true)
                    canvas[y_int][x_int] = '.';
                else 
                    canvas[y_int][x_int] = SHADES[(int) (abs(normal_dot_light) * 12)];

                depth_buffer[y_int][x_int] = z;
            }
        }
    }

}

float compute_delta_time()
{
    return 0;
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
        this_thread::sleep_for(chrono::milliseconds(100));

        // Reset cursor back to draw position
        cout << "\033[" << RESOLUTION << "A";
    }
    

    return 0;
}