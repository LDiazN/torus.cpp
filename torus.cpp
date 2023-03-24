#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <assert.h>
#include <cmath>

using namespace std;


const int RESOLUTION = 20;

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
    out_z = tube_radius * sin(theta);
}

void torus_normal(float theta, float phi, float torus_radius, float tube_radius, float &out_x, float &out_y, float &out_z)
{
    
}

void update_canvas(char (&canvas)[RESOLUTION][RESOLUTION], float delta_time)
{

    cout << delta_time << endl;
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
    while (true)
    {
        // Compute delta time: the time between the last frame start and now
        auto now = chrono::high_resolution_clock::now();
        auto delta_time = chrono::duration_cast<chrono::microseconds>(now - frame_start).count() / 1000000.0f;
        frame_start = now;

        // Update canvas based on delta time
        update_canvas(canvas, delta_time);

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