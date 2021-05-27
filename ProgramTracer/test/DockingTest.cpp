//
// Created by Albert on 2021/5/27.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int main() {
    int *ptr = new int(10);
    delete ptr;
    return 0;
}