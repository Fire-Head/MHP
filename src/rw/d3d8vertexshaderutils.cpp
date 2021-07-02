/****************************************************************************
 *                                                                          *
 * module : d3d8vertexshaderutils.c                                         *
 *                                                                          *
 * purpose:                                                                 *
 *                                                                          *
 ****************************************************************************/

/****************************************************************************
 includes
 */
#include "../rw.h"

#include "d3d8vertexshaderutils.h"

/****************************************************************************
 Global variables
 */

/****************************************************************************
 local defines
 */

#if (defined(__GNUC__) && defined(__cplusplus))
#define D3DMatrixInitMacro(_XX, _XY, _XZ, _XW,  \
                           _YX, _YY, _YZ, _YW,  \
                           _ZX, _ZY, _ZZ, _ZW,  \
                           _WX, _WY, _WZ, _WW ) \
  {                                             \
    { { {   (_XX), (_XY), (_XZ), (_XW) },       \
        {   (_YX), (_YY), (_YZ), (_YW) },       \
        {   (_ZX), (_ZY), (_ZZ), (_ZW) },       \
        {   (_WX), (_WY), (_WZ), (_WW) }        \
    } }                                         \
  }
#endif /* (defined(__GNUC__) && defined(__cplusplus)) */


#if (!defined(D3DMatrixInitMacro))
#define D3DMatrixInitMacro(_XX, _XY, _XZ, _XW,  \
                           _YX, _YY, _YZ, _YW,  \
                           _ZX, _ZY, _ZZ, _ZW,  \
                           _WX, _WY, _WZ, _WW ) \
  {                                             \
     (_XX), (_XY), (_XZ), (_XW),                \
     (_YX), (_YY), (_YZ), (_YW),                \
     (_ZX), (_ZY), (_ZZ), (_ZW),                \
     (_WX), (_WY), (_WZ), (_WW)                 \
  }
#endif /* (!defined(D3DMatrixInitMacro)) */

/****************************************************************************
 local types
 */

/****************************************************************************
 Static variables
 */
static RWD3D8_ALIGN16 D3DMATRIX D3D8ViewProjTransform = {0};

static const RwMatrix *D3D8ActiveTransform = NULL;
static const RwMatrix *D3D8ActiveInverseTransform = NULL;
static const RwMatrix *D3D8ActiveInverseNormalizedTransform = NULL;
static const RwMatrix *D3D8ActiveNormalizedTransform = NULL;

static RWD3D8_ALIGN16 D3DMATRIX   D3D8Transform =
    D3DMatrixInitMacro(1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);

static RWD3D8_ALIGN16 RwMatrix    D3D8InverseTransform =
{
    1.0f, 0.0f, 0.0f, 0,
    0.0f, 1.0f, 0.0f, 0,
    0.0f, 0.0f, 1.0f, 0,
    0.0f, 0.0f, 0.0f, 0x3f800000
};

static RWD3D8_ALIGN16 RwMatrix    D3D8InverseNormalizedTransform =
{
    1.0f, 0.0f, 0.0f, 0,
    0.0f, 1.0f, 0.0f, 0,
    0.0f, 0.0f, 1.0f, 0,
    0.0f, 0.0f, 0.0f, 0x3f800000
};

static RWD3D8_ALIGN16 RwMatrix    D3D8NormalizedTransform =
{
    1.0f, 0.0f, 0.0f, 0,
    0.0f, 1.0f, 0.0f, 0,
    0.0f, 0.0f, 1.0f, 0,
    0.0f, 0.0f, 0.0f, 0x3f800000
};

static const RWD3D8_ALIGN16 RwMatrix D3D8IdentityMatrix =
{
    1.0f, 0.0f, 0.0f, rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL,
    0.0f, 1.0f, 0.0f, 0,
    0.0f, 0.0f, 1.0f, 0,
    0.0f, 0.0f, 0.0f, 0x3f800000
};

D3DMATRIX * RwD3D8GetViewTransform()
{
	static D3DMATRIX ViewTransform;
	RwD3D8GetTransform(D3DTS_VIEW, &ViewTransform);
	return &ViewTransform;
}

D3DMATRIX * RwD3D8GetProjTransform()
{
	static D3DMATRIX ProjTransform;
	RwD3D8GetTransform(D3DTS_PROJECTION, &ProjTransform);
	return &ProjTransform;
}

/****************************************************************************
 *
 * A faster matrix inversion function for orthonormal matrices
 * Tranposition
 *
 */
static void
D3D8MatrixInvertOrthoNormalized(RwMatrix *dst,
                                const RwMatrix *src)
{
    RWFUNCTION(RWSTRING("D3D8MatrixInvertOrthoNormalized"));
    RWASSERT(dst);
    RWASSERT(RWMATRIXALIGNMENT(dst));
    RWASSERT(src);
    RWASSERT(RWMATRIXALIGNMENT(src));
    RWASSERT(rwMatrixIsNormal(src, 0.01f) && rwMatrixIsOrthogonal(src, 0.01f));
    /*
     * Inverse of upper left 3x3 sub matrix
     * is a simple transpose
     */
    /*
     * calculate translation componennt of inverse
     */
    dst->pos.x = -RwV3dDotProductMacro(&src->pos, &src->right);
    dst->right.x = src->right.x;
    dst->up.x = src->right.y;
    dst->at.x = src->right.z;

    dst->pos.y = -RwV3dDotProductMacro(&src->pos, &src->up);
    dst->right.y = src->up.x;
    dst->up.y = src->up.y;
    dst->at.y = src->up.z;

    dst->pos.z = -RwV3dDotProductMacro(&src->pos, &src->at);
    dst->right.z = src->at.x;
    dst->up.z = src->at.y;
    dst->at.z = src->at.z;

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSSetActiveWorldMatrix

*/
void
_rwD3D8VSSetActiveWorldMatrix(const RwMatrix *worldMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSSetActiveWorldMatrix"));

    D3D8ActiveTransform = worldMatrix;
    if (D3D8ActiveTransform != NULL)
    {
        D3D8Transform.m[0][0] = D3D8ActiveTransform->right.x;
        D3D8Transform.m[0][1] = D3D8ActiveTransform->right.y;
        D3D8Transform.m[0][2] = D3D8ActiveTransform->right.z;

        D3D8Transform.m[1][0] = D3D8ActiveTransform->up.x;
        D3D8Transform.m[1][1] = D3D8ActiveTransform->up.y;
        D3D8Transform.m[1][2] = D3D8ActiveTransform->up.z;

        D3D8Transform.m[2][0] = D3D8ActiveTransform->at.x;
        D3D8Transform.m[2][1] = D3D8ActiveTransform->at.y;
        D3D8Transform.m[2][2] = D3D8ActiveTransform->at.z;

        D3D8Transform.m[3][0] = D3D8ActiveTransform->pos.x;
        D3D8Transform.m[3][1] = D3D8ActiveTransform->pos.y;
        D3D8Transform.m[3][2] = D3D8ActiveTransform->pos.z;
    }

    D3D8ActiveInverseTransform = NULL;
    D3D8ActiveInverseNormalizedTransform = NULL;
    D3D8ActiveNormalizedTransform = NULL;

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetComposedTransformMatrix

*/
void
_rwD3D8VSGetComposedTransformMatrix(void *transformMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetComposedTransformMatrix"));
	
	D3DXMatrixMultiply((D3DXMATRIX *)&D3D8ViewProjTransform, (D3DXMATRIX *)&_RwD3D8D3D8ViewTransform, (D3DXMATRIX *)&_RwD3D8D3D8ProjTransform);

    if (D3D8ActiveTransform != NULL)
    {
        D3DXMatrixMultiplyTranspose((D3DXMATRIX *)transformMatrix, (D3DXMATRIX *)&D3D8Transform, (D3DXMATRIX *)&D3D8ViewProjTransform);
    }
    else
    {
        D3DXMatrixTranspose((D3DXMATRIX *)transformMatrix, (D3DXMATRIX *)&D3D8ViewProjTransform);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldViewTransposedMatrix

*/
void
_rwD3D8VSGetWorldViewTransposedMatrix(void *worldViewMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldViewTransposedMatrix"));
    RWASSERT(worldViewMatrix != NULL);

    if (D3D8ActiveTransform != NULL)
    {
        D3DXMatrixMultiplyTranspose((D3DXMATRIX *)worldViewMatrix, (D3DXMATRIX *)&D3D8Transform, (D3DXMATRIX *)&_RwD3D8D3D8ViewTransform);
    }
    else
    {
        D3DXMatrixTranspose((D3DXMATRIX *)worldViewMatrix, (D3DXMATRIX *)&_RwD3D8D3D8ViewTransform);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldViewMatrix

*/
void
_rwD3D8VSGetWorldViewMatrix(void *worldViewMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldViewMatrix"));
    RWASSERT(worldViewMatrix != NULL);

    if (D3D8ActiveTransform != NULL)
    {
        RwMatrixMultiply((RwMatrix *)worldViewMatrix, D3D8ActiveTransform, (RwMatrix *)&_RwD3D8D3D8ViewTransform);

        ((D3DMATRIX *)worldViewMatrix)->_14 = 0.0f;
        ((D3DMATRIX *)worldViewMatrix)->_24 = 0.0f;
        ((D3DMATRIX *)worldViewMatrix)->_34 = 0.0f;
        ((D3DMATRIX *)worldViewMatrix)->_44 = 1.0f;
    }
    else
    {
        memcpy(worldViewMatrix, &_RwD3D8D3D8ViewTransform, sizeof(D3DMATRIX));
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetInverseWorldMatrix

*/
void
_rwD3D8VSGetInverseWorldMatrix(void *inverseWorldMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetInverseWorldMatrix"));

    if (D3D8ActiveInverseTransform == NULL)
    {
        if (D3D8ActiveTransform == NULL ||
            (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) == rwMATRIXINTERNALIDENTITY)
        {
            RWASSERT(D3D8ActiveTransform == NULL || rwMatrixIsIdentity(D3D8ActiveTransform, 0.01f));

            D3D8ActiveInverseTransform = &D3D8IdentityMatrix;
            D3D8ActiveInverseNormalizedTransform = &D3D8IdentityMatrix;
        }
        else if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            D3D8MatrixInvertOrthoNormalized(&D3D8InverseTransform, D3D8ActiveTransform);

            D3D8ActiveInverseTransform = &D3D8InverseTransform;
            D3D8ActiveInverseNormalizedTransform = &D3D8InverseTransform;
        }
        else
        {
            RwMatrixInvert(&D3D8InverseTransform, D3D8ActiveTransform);
            ((D3DMATRIX *)&D3D8InverseTransform)->_14 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_24 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_34 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_44 = 1.0f;

            D3D8ActiveInverseTransform = &D3D8InverseTransform;
        }
    }

    memcpy(inverseWorldMatrix, D3D8ActiveInverseTransform, sizeof(D3DMATRIX));

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldMultiplyMatrix

*/
void
_rwD3D8VSGetWorldMultiplyMatrix(RwMatrix *worldMultiplyMatrix,
                                const RwMatrix *matrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldMultiplyMatrix"));

    if (D3D8ActiveTransform != NULL)
    {
        RwMatrixMultiply(worldMultiplyMatrix, D3D8ActiveTransform, matrix);
    }
    else
    {
        RwMatrixCopy(worldMultiplyMatrix, matrix);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldMultiplyTransposeMatrix

*/
void
_rwD3D8VSGetWorldMultiplyTransposeMatrix(void *worldMultiplyMatrix,
                                         const RwMatrix *matrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldMultiplyTransposeMatrix"));

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) != rwMATRIXINTERNALIDENTITY)
    {
        RwMatrix tmpMtx;

        RwMatrixMultiply(&tmpMtx, D3D8ActiveTransform, matrix);

        D3DXMatrixTranspose((D3DXMATRIX *)worldMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }
    else
    {
        D3DXMatrixTranspose((D3DXMATRIX *)worldMultiplyMatrix, (D3DXMATRIX *)matrix);
    }

    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][0] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][1] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][2] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][3] = 1.0f;

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldViewMultiplyTransposeMatrix

*/
void
_rwD3D8VSGetWorldViewMultiplyTransposeMatrix(void *worldViewMultiplyMatrix,
                                             const RwMatrix *matrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldViewMultiplyTransposeMatrix"));

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) != rwMATRIXINTERNALIDENTITY)
    {
        RwMatrix worldWiew;
        RWD3D8_ALIGN16 D3DMATRIX tmpMtx;

        RwMatrixMultiply(&worldWiew, D3D8ActiveTransform, (RwMatrix *)&_RwD3D8D3D8ViewTransform);

        RwMatrixMultiply((RwMatrix *)&tmpMtx, &worldWiew, matrix);

        tmpMtx.m[0][3] = 0.0f;
        tmpMtx.m[1][3] = 0.0f;
        tmpMtx.m[2][3] = 0.0f;
        tmpMtx.m[3][3] = 1.0f;

        D3DXMatrixTranspose((D3DXMATRIX *)worldViewMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }
    else
    {
        RWD3D8_ALIGN16 D3DMATRIX tmpMtx;

        RwMatrixMultiply((RwMatrix *)&tmpMtx, (RwMatrix *)&_RwD3D8D3D8ViewTransform, matrix);

        tmpMtx.m[0][3] = 0.0f;
        tmpMtx.m[1][3] = 0.0f;
        tmpMtx.m[2][3] = 0.0f;
        tmpMtx.m[3][3] = 1.0f;

        D3DXMatrixTranspose((D3DXMATRIX *)worldViewMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldNormalizedMultiplyTransposeMatrix

*/
void
_rwD3D8VSGetWorldNormalizedMultiplyTransposeMatrix(void *worldMultiplyMatrix,
                                                   const RwMatrix *matrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldNormalizedMultiplyTransposeMatrix"));

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) != rwMATRIXINTERNALIDENTITY)
    {
        RwMatrix tmpMtx;

        if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            RwMatrixMultiply(&tmpMtx, D3D8ActiveTransform, matrix);
        }
        else
        {
            if (D3D8ActiveNormalizedTransform == NULL)
            {
                RwMatrixOrthoNormalize(&D3D8NormalizedTransform, D3D8ActiveTransform);
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_14 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_24 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_34 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_44 = 1.0f;

                D3D8ActiveNormalizedTransform = &D3D8NormalizedTransform;
            }

            RwMatrixMultiply(&tmpMtx, D3D8ActiveNormalizedTransform, matrix);
        }

        D3DXMatrixTranspose((D3DXMATRIX *)worldMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }
    else
    {
        D3DXMatrixTranspose((D3DXMATRIX *)worldMultiplyMatrix, (D3DXMATRIX *)matrix);
    }

    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][0] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][1] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][2] = 0.0f;
    ((D3DMATRIX *)worldMultiplyMatrix)->m[3][3] = 1.0f;

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldNormalizedViewMultiplyTransposeMatrix

*/
void
_rwD3D8VSGetWorldNormalizedViewMultiplyTransposeMatrix(void *worldViewMultiplyMatrix,
                                                       const RwMatrix *matrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldNormalizedViewMultiplyTransposeMatrix"));

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) != rwMATRIXINTERNALIDENTITY)
    {
        RwMatrix worldWiew;
        RWD3D8_ALIGN16 D3DMATRIX tmpMtx;

        if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            RwMatrixMultiply(&worldWiew, D3D8ActiveTransform, (RwMatrix *)&_RwD3D8D3D8ViewTransform);
        }
        else
        {
            if (D3D8ActiveNormalizedTransform == NULL)
            {
                RwMatrixOrthoNormalize(&D3D8NormalizedTransform, D3D8ActiveTransform);
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_14 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_24 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_34 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_44 = 1.0f;

                D3D8ActiveNormalizedTransform = &D3D8NormalizedTransform;
            }

            RwMatrixMultiply(&worldWiew, D3D8ActiveNormalizedTransform, (RwMatrix *)&_RwD3D8D3D8ViewTransform);
        }

        RwMatrixMultiply((RwMatrix *)&tmpMtx, &worldWiew, matrix);

        tmpMtx.m[0][3] = 0.0f;
        tmpMtx.m[1][3] = 0.0f;
        tmpMtx.m[2][3] = 0.0f;
        tmpMtx.m[3][3] = 1.0f;

        D3DXMatrixTranspose((D3DXMATRIX *)worldViewMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }
    else
    {
        RWD3D8_ALIGN16 D3DMATRIX tmpMtx;

        RwMatrixMultiply((RwMatrix *)&tmpMtx, (RwMatrix *)&_RwD3D8D3D8ViewTransform, matrix);

        tmpMtx.m[0][3] = 0.0f;
        tmpMtx.m[1][3] = 0.0f;
        tmpMtx.m[2][3] = 0.0f;
        tmpMtx.m[3][3] = 1.0f;

        D3DXMatrixTranspose((D3DXMATRIX *)worldViewMultiplyMatrix, (D3DXMATRIX *)&tmpMtx);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetWorldNormalizedTransposeMatrix

 On entry :
 On exit  :
 */
void
_rwD3D8VSGetWorldNormalizedTransposeMatrix(void *worldMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetWorldNormalizedTransposeMatrix"));
    RWASSERT(worldMatrix != NULL);

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) != rwMATRIXINTERNALIDENTITY)
    {
        if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            D3DXMatrixTranspose((D3DXMATRIX *)worldMatrix, (D3DXMATRIX *)&D3D8Transform);
        }
        else
        {
            if (D3D8ActiveNormalizedTransform == NULL)
            {
                RwMatrixOrthoNormalize(&D3D8NormalizedTransform, D3D8ActiveTransform);
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_14 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_24 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_34 = 0.0f;
                ((D3DMATRIX *)&D3D8NormalizedTransform)->_44 = 1.0f;

                D3D8ActiveNormalizedTransform = &D3D8NormalizedTransform;
            }

            D3DXMatrixTranspose((D3DXMATRIX *)worldMatrix, (D3DXMATRIX *)D3D8ActiveNormalizedTransform);
        }
    }
    else
    {
        memcpy(worldMatrix, &D3D8IdentityMatrix, sizeof(D3DMATRIX));
        ((D3DMATRIX *)worldMatrix)->_14 = 0.0f;
        ((D3DMATRIX *)worldMatrix)->_24 = 0.0f;
        ((D3DMATRIX *)worldMatrix)->_34 = 0.0f;
        ((D3DMATRIX *)worldMatrix)->_44 = 1.0f;
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetProjectionTransposedMatrix

*/
void
_rwD3D8VSGetProjectionTransposedMatrix(void *projectionMatrix)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetProjectionTransposedMatrix"));

    D3DXMatrixTranspose((D3DXMATRIX *)projectionMatrix, (D3DXMATRIX *)&_RwD3D8D3D8ProjTransform);

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetNormalInLocalSpace

 On entry :
 On exit  :
 */
void
_rwD3D8VSGetNormalInLocalSpace(const RwV3d *normalWorldSpace,
                               RwV3d *normalLocalSpace)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetNormalInLocalSpace"));
    RWASSERT(normalWorldSpace != NULL);
    RWASSERT(normalLocalSpace != NULL);

    if (D3D8ActiveInverseNormalizedTransform == NULL)
    {
        if (D3D8ActiveTransform == NULL ||
            (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) == rwMATRIXINTERNALIDENTITY)
        {
            RWASSERT(D3D8ActiveTransform == NULL || rwMatrixIsIdentity(D3D8ActiveTransform, 0.01f));

            D3D8ActiveInverseTransform = &D3D8IdentityMatrix;
            D3D8ActiveInverseNormalizedTransform = &D3D8IdentityMatrix;
        }
        else if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            if (D3D8ActiveInverseTransform == NULL)
            {
                D3D8MatrixInvertOrthoNormalized(&D3D8InverseTransform, D3D8ActiveTransform);

                D3D8ActiveInverseTransform = &D3D8InverseTransform;
            }

            D3D8ActiveInverseNormalizedTransform = D3D8ActiveInverseTransform;
        }
        else
        {
            if (D3D8ActiveInverseTransform == NULL)
            {
                RwMatrixInvert(&D3D8InverseTransform, D3D8ActiveTransform);
                ((D3DMATRIX *)&D3D8InverseTransform)->_14 = 0.0f;
                ((D3DMATRIX *)&D3D8InverseTransform)->_24 = 0.0f;
                ((D3DMATRIX *)&D3D8InverseTransform)->_34 = 0.0f;
                ((D3DMATRIX *)&D3D8InverseTransform)->_44 = 1.0f;

                D3D8ActiveInverseTransform = &D3D8InverseTransform;
            }

            RwMatrixOrthoNormalize(&D3D8InverseNormalizedTransform, D3D8ActiveInverseTransform);
            ((D3DMATRIX *)&D3D8InverseNormalizedTransform)->_14 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseNormalizedTransform)->_24 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseNormalizedTransform)->_34 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseNormalizedTransform)->_44 = 1.0f;

            D3D8ActiveInverseNormalizedTransform = &D3D8InverseNormalizedTransform;
        }
    }

    if (D3D8ActiveInverseNormalizedTransform != &D3D8IdentityMatrix)
    {
        D3DXVec3TransformNormal( (D3DXVECTOR3 *)normalLocalSpace,
                                 (D3DXVECTOR3 *)normalWorldSpace,
                                 (const D3DXMATRIX *)D3D8ActiveInverseNormalizedTransform );
    }
    else
    {
        normalLocalSpace->x = normalWorldSpace->x;
        normalLocalSpace->y = normalWorldSpace->y;
        normalLocalSpace->z = normalWorldSpace->z;
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetPointInLocalSpace

 On entry :
 On exit  :
 */
void
_rwD3D8VSGetPointInLocalSpace(const RwV3d *pointWorldSpace,
                              RwV3d *pointLocalSpace)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetPointInLocalSpace"));
    RWASSERT(pointWorldSpace != NULL);
    RWASSERT(pointLocalSpace != NULL);

    if (D3D8ActiveInverseTransform == NULL)
    {
        if (D3D8ActiveTransform == NULL ||
            (D3D8ActiveTransform->flags & rwMATRIXINTERNALIDENTITY) == rwMATRIXINTERNALIDENTITY)
        {
            RWASSERT(D3D8ActiveTransform == NULL || rwMatrixIsIdentity(D3D8ActiveTransform, 0.01f));

            D3D8ActiveInverseTransform = &D3D8IdentityMatrix;
        }
        else if ((D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) == rwMATRIXTYPEORTHONORMAL)
        {
            D3D8MatrixInvertOrthoNormalized(&D3D8InverseTransform, D3D8ActiveTransform);

            D3D8ActiveInverseTransform = &D3D8InverseTransform;
        }
        else
        {
            RwMatrixInvert(&D3D8InverseTransform, D3D8ActiveTransform);
            ((D3DMATRIX *)&D3D8InverseTransform)->_14 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_24 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_34 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_44 = 1.0f;

            D3D8ActiveInverseTransform = &D3D8InverseTransform;
        }
    }

    if (D3D8ActiveInverseTransform != &D3D8IdentityMatrix)
    {
        D3DXVec3TransformCoord((D3DXVECTOR3 *)pointLocalSpace,
                               (D3DXVECTOR3 *)pointWorldSpace,
                               (const D3DXMATRIX *)D3D8ActiveInverseTransform );
    }
    else
    {
        pointLocalSpace->x = pointWorldSpace->x;
        pointLocalSpace->y = pointWorldSpace->y;
        pointLocalSpace->z = pointWorldSpace->z;
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwD3D8VSGetRadiusInLocalSpace

 On entry :
 On exit  :
 */
void
_rwD3D8VSGetRadiusInLocalSpace(RwReal radiusWorldSpace,
                               RwReal *radiusLocalSpace)
{
    RWFUNCTION(RWSTRING("_rwD3D8VSGetRadiusInLocalSpace"));
    RWASSERT(radiusLocalSpace != NULL);

    if (D3D8ActiveTransform != NULL &&
        (D3D8ActiveTransform->flags & rwMATRIXTYPEORTHONORMAL) != rwMATRIXTYPEORTHONORMAL)
    {
        RwV3d radiusVector;
        RwReal squareLength, invlength;

        if (D3D8ActiveInverseTransform == NULL)
        {
            RwMatrixInvert(&D3D8InverseTransform, D3D8ActiveTransform);
            ((D3DMATRIX *)&D3D8InverseTransform)->_14 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_24 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_34 = 0.0f;
            ((D3DMATRIX *)&D3D8InverseTransform)->_44 = 1.0f;

            D3D8ActiveInverseTransform = &D3D8InverseTransform;
        }

        radiusVector.z = radiusVector.y = radiusVector.x = 0.57735026918962576450914878050196f;

        D3DXVec3TransformNormal((D3DXVECTOR3 *)&radiusVector,
                                (D3DXVECTOR3 *)&radiusVector,
                                (D3DXMATRIX *)D3D8ActiveInverseTransform );

        squareLength = RwV3dDotProductMacro(&radiusVector, &radiusVector);
        rwInvSqrtMacro(&invlength, squareLength);
        *radiusLocalSpace = radiusWorldSpace * invlength;
    }
    else
    {
        *radiusLocalSpace = radiusWorldSpace;
    }

    RWRETURNVOID();
}
