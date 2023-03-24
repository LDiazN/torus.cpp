#include <iostream>
#include <chrono>
#include <thread>
using namespace std;


const int X_RESOLUTION = 20;
const int Y_RESOLUTION = 20;


void update_canvas(char (&canvas)[X_RESOLUTION][Y_RESOLUTION], int x_pos_i, int x_pos_j)
{
    for (int j = 0; j < X_RESOLUTION; j++ )
        for(int i = 0; i < Y_RESOLUTION; i++)
            if (j == x_pos_j && i == x_pos_i)
                canvas[i][j] = 'x';
            else 
                canvas[i][j] = ' ';

}

int main() {
    char canvas[X_RESOLUTION][Y_RESOLUTION];



    // Update loop
    int x_pos_i = 0, x_pos_j = 0;

    while (true)
    {

        if (x_pos_i == X_RESOLUTION - 1)
            x_pos_j = (x_pos_j + 1) % Y_RESOLUTION;

        x_pos_i = (x_pos_i + 1) % X_RESOLUTION;

        update_canvas(canvas, x_pos_i, x_pos_j);

        // print canvas to terminal
        for (int i = 0; i < Y_RESOLUTION; i++)
        {
            for (int j = 0; j < X_RESOLUTION; j++)
            {
                cout << canvas[i][j] << " ";
            }
            cout << endl;
        }

        cout << flush;
        this_thread::sleep_for(chrono::milliseconds(100));
        cout << "\033[" << X_RESOLUTION << "A";
    }
    

    return 0;
}