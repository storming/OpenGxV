#ifndef __GV_TRANSFORM_H__
#define __GV_TRANSFORM_H__

#include "gv_object.h"
#include "gv_math.h"

GV_NS_BEGIN

class Transform {
    friend class DisplayObject;
public:
    Transform() noexcept {
        _matrix = new Matrix;
        _matrix->setIdentity();
        _concatenatedMatrix = new Matrix;
    }
    Matrix *matrix() noexcept {
        return &_matrix;
    }
    void matrix(Matrix *mat) noexcept {
        if (!mat){
            return;
        }
        if (mat != &_matrix){
            _matrix = mat;
        }
        _matrixDirty = true;
        _boundsDirty = true;
    }
    const Matrix *concatenatedMatrix() noexcept {
        return _concatenatedMatrix;
    }
private:
}; 

GV_NS_END


#endif

