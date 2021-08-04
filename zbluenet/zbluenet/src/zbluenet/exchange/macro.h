#ifndef ZBLUENET_EXCHANGE_MACRO_H
#define ZBLUENET_EXCHANGE_MACRO_H

#include <stdint.h>
#include <cstring>

#define READ_INT8(_var)             \
    do {                            \
        if (left_bytes < 1) {       \
            return -1;              \
        }                           \
        _var = *(const uint8_t *)p; \
        p += 1;                     \
        left_bytes -= 1;            \
    } while (0)                     \

#define WRITE_INT8(_var)               \
    do {                               \
        if (left_bytes < 1) {          \
            return -1;                 \
        }                              \
        *(uint8_t *)p = (uint8_t)_var; \
        p += 1;                        \
        left_bytes -=1;                \
    } while (0)                        \

#define READ_INT16(_var)                               \
    do {                                               \
        if (left_bytes < 2) {                          \
            return -1;                                 \
        }                                              \
        _var = (uint16_t)(*(const uint8_t *)(p + 1)) | \
               (uint16_t)(*(const uint8_t *)(p)) << 8; \
        p += 2;                                        \
        left_bytes -= 2;                               \
    } while (0)                                        \

#define WRITE_INT16(_var)                       \
    do {                                        \
        if (left_bytes < 2) {                   \
            return -1;                          \
        }                                       \
        *(uint8_t *)(p) = (uint8_t)(_var >> 8); \
        *(uint8_t *)(p + 1) = (uint8_t)(_var);  \
        p += 2;                                 \
        left_bytes -= 2;                        \
    } while (0)                                 \

#define READ_INT32(_var)                                     \
    do {                                                     \
        if (left_bytes < 4) {                                \
            return -1;                                       \
        }                                                    \
        _var = (uint32_t)(*(const uint8_t *)(p + 3)) |       \
               (uint32_t)(*(const uint8_t *)(p + 2)) << 8 |  \
               (uint32_t)(*(const uint8_t *)(p + 1)) << 16 | \
               (uint32_t)(*(const uint8_t *)(p)) << 24;      \
        p += 4;                                              \
        left_bytes -= 4;                                     \
    } while (0)                                              \

#define WRITE_INT32(_var)                            \
    do {                                             \
        if (left_bytes < 4) {                        \
            return -1;                               \
        }                                            \
        *(uint8_t *)(p) = (uint8_t)(_var >> 24);     \
        *(uint8_t *)(p + 1) = (uint8_t)(_var >> 16); \
        *(uint8_t *)(p + 2) = (uint8_t)(_var >> 8);  \
        *(uint8_t *)(p + 3) = (uint8_t)(_var);       \
        p += 4;                                      \
        left_bytes -= 4;                             \
    } while (0)                                      \

#define READ_INT64(_var)                                     \
    do {                                                     \
        if (left_bytes < 8) {                                \
            return -1;                                       \
        }                                                    \
        _var = (uint64_t)(*(const uint8_t *)(p + 7)) |       \
               (uint64_t)(*(const uint8_t *)(p + 6)) << 8 |  \
               (uint64_t)(*(const uint8_t *)(p + 5)) << 16 | \
               (uint64_t)(*(const uint8_t *)(p + 4)) << 24 | \
               (uint64_t)(*(const uint8_t *)(p + 3)) << 32 | \
               (uint64_t)(*(const uint8_t *)(p + 2)) << 40 | \
               (uint64_t)(*(const uint8_t *)(p + 1)) << 48 | \
               (uint64_t)(*(const uint8_t *)(p)) << 56;      \
        p += 8;                                              \
        left_bytes -= 8;                                     \
    } while (0)                                              \

#define WRITE_INT64(_var)                            \
    do {                                             \
        if (left_bytes < 8) {                        \
            return -1;                               \
        }                                            \
        *(uint8_t *)(p) = (uint8_t)(_var >> 56);     \
        *(uint8_t *)(p + 1) = (uint8_t)(_var >> 48); \
        *(uint8_t *)(p + 2) = (uint8_t)(_var >> 40); \
        *(uint8_t *)(p + 3) = (uint8_t)(_var >> 32); \
        *(uint8_t *)(p + 4) = (uint8_t)(_var >> 24); \
        *(uint8_t *)(p + 5) = (uint8_t)(_var >> 16); \
        *(uint8_t *)(p + 6) = (uint8_t)(_var >> 8);  \
        *(uint8_t *)(p + 7) = (uint8_t)(_var);       \
        p += 8;                                      \
        left_bytes -= 8;                             \
    } while (0)                                      \

#define READ_INT16V(_var)           \
    do {                            \
        READ_INT8(_var);            \
        if ((uint16_t)_var < 255) { \
            break;                  \
        } else {                    \
            READ_INT16(_var);       \
        }                           \
    } while (0)                     \

#define WRITE_INT16V(_var)          \
    do {                            \
        if ((uint16_t)_var < 255) { \
            WRITE_INT8(_var);       \
        } else {                    \
            WRITE_INT8(255);        \
            WRITE_INT16(_var);      \
        }                           \
    } while (0)                     \

#define READ_INT32V(_var)                   \
    do {                                    \
        READ_INT8(_var);                    \
        if ((uint32_t)_var < 254) {         \
            break;                          \
        } else if ((uint32_t)_var == 254) { \
            READ_INT16(_var);               \
        } else {                            \
            READ_INT32(_var);               \
        }                                   \
    } while (0)                             \

#define WRITE_INT32V(_var)                     \
    do {                                       \
        if ((uint32_t)_var < 254) {            \
            WRITE_INT8(_var);                  \
        } else if ((uint32_t)_var <= 0xffff) { \
            WRITE_INT8(254);                   \
            WRITE_INT16(_var);                 \
        } else {                               \
            WRITE_INT8(255);                   \
            WRITE_INT32(_var);                 \
        }                                      \
    } while (0)                                \

#define READ_INT64V(_var)                   \
    do {                                    \
        READ_INT8(_var);                    \
        if ((uint64_t)_var < 253) {         \
            break;                          \
        } else if ((uint64_t)_var == 253) { \
            READ_INT16(_var);               \
        } else if ((uint64_t)_var == 254) { \
            READ_INT32(_var);               \
        } else {                            \
            READ_INT64(_var);               \
        }                                   \
    } while (0)                             \

#define WRITE_INT64V(_var)                         \
    do {                                           \
        if ((uint64_t)_var < 253) {                \
            WRITE_INT8(_var);                      \
        } else if ((uint64_t)_var <= 0xffff) {     \
            WRITE_INT8(253);                       \
            WRITE_INT16(_var);                     \
        } else if ((uint64_t)_var <= 0xffffffff) { \
            WRITE_INT8(254);                       \
            WRITE_INT32(_var);                     \
        } else {                                   \
            WRITE_INT8(255);                       \
            WRITE_INT64(_var);                     \
        }                                          \
    } while (0)                                    \

#define READ_ENUM(_var, _enum_type) \
    do {                            \
        int32_t v;                  \
        READ_INT32V(v);             \
        _var = (_enum_type)v;       \
    } while (0)                     \

#define WRITE_ENUM(_var) WRITE_INT32V(_var)

#define READ_LENGTH(_length)         \
    do {                             \
        READ_INT8(_length);          \
        if (_length < 254) {         \
            break;                   \
        } else if (_length == 254) { \
            READ_INT16(_length);     \
        } else {                     \
            READ_INT32(_length);     \
        }                            \
    } while (0)                      \

#define WRITE_LENGTH(_length)               \
    do {                                    \
        if (_length < 254) {                \
            WRITE_INT8(_length);            \
        } else if (_length <= 0xffff) {     \
            WRITE_INT8(254);                \
            WRITE_INT16(_length);           \
        } else if (_length <= 0xffffffff) { \
            WRITE_INT8(255);                \
            WRITE_INT32(_length);           \
        } else {                            \
            return -1;                      \
        }                                   \
    } while (0)                             \

#define READ_STRING(_var)          \
    do {                           \
        size_t length;             \
        READ_LENGTH(length);       \
        if (left_bytes < length) { \
            return -1;             \
        }                          \
        _var.assign(p, length);    \
        p += length;               \
        left_bytes -= length;      \
    } while (0)                    \

#define WRITE_STRING(_var)                      \
    do {                                        \
        WRITE_LENGTH(_var.size());              \
        if (left_bytes < _var.size()) {         \
            return -1;                          \
        }                                       \
        ::memcpy(p, _var.c_str(), _var.size()); \
        p += _var.size();                       \
        left_bytes -= _var.size();              \
    } while (0)                                 \

#define READ_STRUCT(_var)                             \
    do {                                              \
        int struct_size = _var.decode(p, left_bytes); \
        if (-1 == struct_size) {                      \
            return -1;                                \
        }                                             \
        p += struct_size;                             \
        left_bytes -= struct_size;                    \
    } while (0)                                       \

#define WRITE_STRUCT(_var)                            \
    do {                                              \
        int struct_size = _var.encode(p, left_bytes); \
        if (-1 == struct_size) {                      \
            return -1;                                \
        }                                             \
        p += struct_size;                             \
        left_bytes -= struct_size;                    \
    } while (0)                                       \

#define READ_LIST(_var, _read_func, _list_cpp_type) \
    do {                                            \
        size_t length;                              \
        READ_LENGTH(length);                        \
        _var.clear();                               \
        _var.reserve(length);                       \
        for (size_t i = 0; i < length; ++i) {       \
            _list_cpp_type v;                       \
            _read_func(v);                          \
            _var.push_back(v);                      \
        }                                           \
    } while (0)                                     \

#define READ_ENUM_LIST(_var, _enum_type)      \
    do {                                      \
        size_t length;                        \
        READ_LENGTH(length);                  \
        _var.clear();                         \
        _var.reserve(length);                 \
        for (size_t i = 0; i < length; ++i) { \
            _enum_type v;                     \
            READ_ENUM(v, _enum_type);         \
            _var.push_back(v);                \
        }                                     \
    } while (0)                               \

#define WRITE_LIST(_var, _write_func)              \
    do {                                           \
        WRITE_LENGTH(_var.size());                 \
        for (size_t i = 0; i < _var.size(); ++i) { \
            _write_func(_var[i]);                  \
        }                                          \
    } while (0)                                    \

#endif
