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

void torus(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    assert(tube_radius < torus_radius && "Radius of tube should be smaller than radius of torus");
    out_x = (torus_radius + tube_radius * cos(theta)) * cos(phi);
    out_y = (torus_radius + tube_radius * cos(theta)) * sin(phi);
    out_z = -tube_radius * sin(theta);
}

void torus_tangent(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{

}

void torus_bitangent(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{

}

void torus_normal(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    
}

void cross(float x1, float y1, float z1, float x2, float y2, float z2, float& out_x, float& out_y, float& out_z)
{
    out_x = y1 * z2 - z1 * y2;
    out_y = z1 * x1 - x1 * z2;
    out_z = x1 * y2 - y1 * x2;
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

    // Clear canvas so the previous image is removed
    clear_canvas(canvas);
    
    // Clear depth buffer. We store Z^-1 instead of Z, so 0 represents infinite distance
    for(int i = 0; i < RESOLUTION; i++)
        for(int j = 0; j < RESOLUTION; j++)
            depth_buffer[i][j] = 0;


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
            float x, y, z;

            // Compute this point
            torus(theta, phi,((float) RESOLUTION) / 2.f, ((float) RESOLUTION) / 4.f, x, y, z);

            // Value stored in Depth Buffer
            const float z_inv = z != 0 ? 1 / z : 0;

            // transform this point
            rotate_y(time_passed, x,y,z, x,y,z);
            scale(0.3f, x,y,z, x,y,z);
            translate(5, 5, 13, x,y,z, x,y,z);

            // Project this point to the screen: 
            int x_int, y_int;
            x_int = (int) ((EYE_DISTANCE_TO_SCREEN * x / z));
            y_int = (int) ((EYE_DISTANCE_TO_SCREEN * y / z));

            if (0 <= x_int && x_int < RESOLUTION && 0 <= y_int && y_int < RESOLUTION && depth_buffer[y_int][x_int] > z_inv)
            {
                canvas[y_int][x_int] = '#';
                depth_buffer[y_int][x_int] = z_inv;
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