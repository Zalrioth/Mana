#pragma once
#ifndef TEXTURE_H_
#define TEXTURE_H_

struct Texture {
    unsigned int id;
    char type[2048];
    char path[2048];
};

#endif // TEXTURE_H_