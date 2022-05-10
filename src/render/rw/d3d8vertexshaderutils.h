/****************************************************************************
 *                                                                          *
 * Module  : d3d8vertexshaderutils.h                                        *
 *                                                                          *
 * Purpose : See d3d8vertexshaderutils.c                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef D3D8VERTEXSHADERUTILS_H
#define D3D8VERTEXSHADERUTILS_H

/****************************************************************************
 Includes
 */
#include <d3d8.h>

/****************************************************************************
 Global Defines
 */
#define _RwD3D8D3D8ViewTransform *RwD3D8GetViewTransform()
#define _RwD3D8D3D8ProjTransform *RwD3D8GetProjTransform()

/****************************************************************************
 Global Types
 */

/****************************************************************************
 Global Variables
 */

/****************************************************************************
 Function prototypes
 */

/* RWPUBLIC */
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
/* RWPUBLICEND */

/* RWPUBLIC */

extern D3DMATRIX *
RwD3D8GetViewTransform();

extern D3DMATRIX *
RwD3D8GetProjTransform();

/* Call this function before calling to any of the following ones */
extern void
_rwD3D8VSSetActiveWorldMatrix(const RwMatrix *worldMatrix);

/*
 * Copy the transposed combined world-view-projection matrix to the
 * requested destination.
 */
extern void
_rwD3D8VSGetComposedTransformMatrix(void *transformMatrix);

/*
 * Copy the transposed combined world-view matrix to the
 * requested destination.
 */
extern void
_rwD3D8VSGetWorldViewTransposedMatrix(void *worldViewMatrix);

extern void
_rwD3D8VSGetWorldViewMatrix(void *worldViewMatrix);

extern void
_rwD3D8VSGetInverseWorldMatrix(void *inverseWorldMatrix);

extern void
_rwD3D8VSGetWorldMultiplyMatrix(RwMatrix *worldMultiplyMatrix,
                                const RwMatrix *matrix);

extern void
_rwD3D8VSGetWorldMultiplyTransposeMatrix(void *worldMultiplyMatrix,
                                         const RwMatrix *matrix);

extern void
_rwD3D8VSGetWorldViewMultiplyTransposeMatrix(void *worldViewMultiplyMatrix,
                                             const RwMatrix *matrix);

extern void
_rwD3D8VSGetWorldNormalizedMultiplyTransposeMatrix(void *worldMultiplyMatrix,
                                                   const RwMatrix *matrix);

extern void
_rwD3D8VSGetWorldNormalizedViewMultiplyTransposeMatrix(void *worldViewMultiplyMatrix,
                                                       const RwMatrix *matrix);

extern void
_rwD3D8VSGetWorldNormalizedTransposeMatrix(void *worldMatrix);

extern void
_rwD3D8VSGetProjectionTransposedMatrix(void *projectionMatrix);

/* Converting light parameters to local space */
extern void
_rwD3D8VSGetNormalInLocalSpace(const RwV3d *normalWorldSpace,
                               RwV3d *normalLocalSpace);

extern void
_rwD3D8VSGetPointInLocalSpace(const RwV3d *pointWorldSpace,
                              RwV3d *pointLocalSpace);

extern void
_rwD3D8VSGetRadiusInLocalSpace(RwReal radiusWorldSpace,
                               RwReal *radiusLocalSpace);

#ifdef    __cplusplus
}
#endif /* __cplusplus */
/* RWPUBLICEND */

#endif /* D3D8VERTEXSHADERUTILS_H */
