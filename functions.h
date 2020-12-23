#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))
#include <algorithm>

float degToRad(float a) {
    return a * M_PI / 180.0;
}

float FixAng(float a) {
    if(a >= 360) {
        a = a - 360;
    } else if(a < 0) {
        a = 360 + a;
    }
    return a;
}

int indexOf(int arr[], int haystack) {
    size_t size = ARRAY_SIZE(arr);
    int *end = arr + size;

    int *result = std::find(arr, end, haystack);
    if (result != end) {
        return reinterpret_cast<size_t>(result);
    }

    return -1;
}