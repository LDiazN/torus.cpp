#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
using namespace std;


const int RESOLUTION = 20;


void clear_canvas(char (&canvas)[RESOLUTION][RESOLUTION])
{
    for(int i = 0; i < RESOLUTION; i++)
        for(int j = 0; j < RESOLUTION; j++)
            canvas[i][j] = ' ';
}

void update_canvas(char (&canvas)[RESOLUTION][RESOLUTION], float delta_time)
{
    
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