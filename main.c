/* 
 * Code passages to read as part of lab.
 */
 #include <stdio.h>
 #include <stdlib.h>

void goofy(int a[], int* b) {
    b[2] = 101;
    a[2] *= 7;
}

void change_char(char* s) {
    *s = 'C';
    s = "UICFlames";
}

void change_ptr(char** p_str) {
    **p_str = 'j';
    *p_str = "Chicago";
    *p_str = (char*)malloc(4*sizeof(char));
    *p_str = "IL";
    p_str = NULL;
}

void silly(void) {
    char word[6] = "hello";
    char* pw = (char*)malloc(4*sizeof(char));
    pw = word;
    
    change_char(pw);    // can you substitute word for pw?
    change_ptr(&pw);    // can you substitute &word for &pw?
}


int main(int argc, char* argv[]) {
    int arr[] = {0, 10, 20, 30, 40, 50};
    int *ptr = arr;

    goofy(arr, ptr);
    silly();
    return 0;
}
