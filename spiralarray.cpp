#include <iostream>
using namespace std;

int main()
{
    int rows = 4, cols = 5;
    int arr[10][10];

    int top = 0, bottom = rows - 1; //bottom ki value= 3
    int left = 0, right = cols - 1; // right ki value = 4

    int num = 1;

    while (top <= bottom && left <= right)
    {

        //  Left to Right ke liye
        for (int i = left; i <= right; i++)
        {
            arr[top][i] = num++;
        }
        top++;

        //  uper se niche ke liye
        for (int i = top; i <= bottom; i++)
        {
            arr[i][right] = num++;
        }
        right--;

        //  Right to Left ke liye
        for (int i = right; i >= left; i--)
        {
            arr[bottom][i] = num++;
        }
        bottom--;

        //  niche  se uper ke liye
        for (int i = bottom; i >= top; i--)
        {
            arr[i][left] = num++;
        }
        left++;
    }
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << arr[i][j] << "\t";
        }
        cout << endl;
    }

    return 0;
}