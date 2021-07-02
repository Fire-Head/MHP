    vs.1.1
    m4x4 oPos.xyzw, v0, c0
    mov r0.y, c9.z
    mad r0.xyz, v0, r0.y, -c7
    dp3 r1.x, r0, r0
    mad r0.w, r1.x, c11.z, c11.x
    rsq r2.w, r1.x
    rcp r1.w, r0.w
    sge r0.w, c9.w, r2.w
    mul r0.xyz, r0, r2.w
    mad r2.w, r0.w, -r1.w, r1.w
    dp3 r1.x, c8, r0
    dp3 r0.x, v1, r0
    mad r1.w, r1.x, c9.x, c9.y
    max r0.w, -r0.x, c4.x
    min r1.w, r1.w, c4.w
    mul r0.w, r2.w, r0.w
    max r1.w, r1.w, c4.x
    mul r0.w, r0.w, r1.w
    mul r1.xyz, r0.w, c10
    mov r0.y, c14.z
    mad r0.xyz, v0, r0.y, -c12
    mov r0.w, c4.x
    mov r2.xyz, c5
    mad r2.xyz, c6, r2, r0.w
    dp3 r3.x, r0, r0
    mad r2.xyz, r1, c5, r2
    mad r0.w, r3.x, c16.z, c16.x
    rsq r2.w, r3.x
    rcp r1.w, r0.w
    sge r0.w, c14.w, r2.w
    mul r0.xyz, r0, r2.w
    mad r2.w, r0.w, -r1.w, r1.w
    dp3 r1.x, c13, r0
    dp3 r0.x, v1, r0
    mad r1.w, r1.x, c14.x, c14.y
    max r0.w, -r0.x, c4.x
    min r1.w, r1.w, c4.w
    mul r0.w, r2.w, r0.w
    max r1.w, r1.w, c4.x
    mul r0.w, r0.w, r1.w
    mov r0.y, c19.z
    mad r0.xyz, v0, r0.y, -c17
    mul r1.xyz, r0.w, c15
    dp3 r3.x, r0, r0
    mad r2.xyz, r1, c5, r2
    mad r0.w, r3.x, c21.z, c21.x
    rsq r2.w, r3.x
    rcp r1.w, r0.w
    sge r0.w, c19.w, r2.w
    mul r0.xyz, r0, r2.w
    mad r2.w, r0.w, -r1.w, r1.w
    dp3 r1.x, c18, r0
    dp3 r0.x, v1, r0
    mad r1.w, r1.x, c19.x, c19.y
    max r0.w, -r0.x, c4.x
    min r1.w, r1.w, c4.w
    mul r0.w, r2.w, r0.w
    max r1.w, r1.w, c4.x
    mul r0.w, r0.w, r1.w
    mov r0.y, c24.z
    mad r0.xyz, v0, r0.y, -c22
    mul r1.xyz, r0.w, c20
    dp3 r3.x, r0, r0
    mad r2.xyz, r1, c5, r2
    mad r0.w, r3.x, c26.z, c26.x
    rsq r2.w, r3.x
    rcp r1.w, r0.w
    sge r0.w, c24.w, r2.w
    mul r0.xyz, r0, r2.w
    mad r2.w, r0.w, -r1.w, r1.w
    dp3 r1.x, c23, r0
    dp3 r0.x, v1, r0
    mad r1.w, r1.x, c24.x, c24.y
    max r0.w, -r0.x, c4.x
    min r1.w, r1.w, c4.w
    mul r0.w, r2.w, r0.w
    max r1.w, r1.w, c4.x
    mul r0.w, r0.w, r1.w
    mov r0.y, c29.z
    mad r0.xyz, v0, r0.y, -c27
    mul r1.xyz, r0.w, c25
    dp3 r3.x, r0, r0
    mad r1.xyz, r1, c5, r2
    mad r0.w, r3.x, c31.z, c31.x
    rsq r2.w, r3.x
    rcp r1.w, r0.w
    sge r0.w, c29.w, r2.w
    mul r0.xyz, r0, r2.w
    mad r1.w, r0.w, -r1.w, r1.w
    dp3 r2.x, v1, r0
    dp3 r0.x, c28, r0
    max r0.w, -r2.x, c4.x
    mul r0.w, r1.w, r0.w
    mad r1.w, r0.x, c29.x, c29.y
    min r1.w, r1.w, c4.w
    mov r0.y, c34.z
    mad r0.xyz, v0, r0.y, -c32
    max r1.w, r1.w, c4.x
    dp3 r2.x, r0, r0
    mul r2.w, r0.w, r1.w
    mad r0.w, r2.x, c36.z, c36.x
    rsq r3.w, r2.x
    rcp r1.w, r0.w
    sge r0.w, c34.w, r3.w
    mul r0.xyz, r0, r3.w
    mad r3.w, r0.w, -r1.w, r1.w
    dp3 r2.x, c33, r0
    dp3 r0.x, v1, r0
    mad r1.w, r2.x, c34.x, c34.y
    max r0.w, -r0.x, c4.x
    min r1.w, r1.w, c4.w
    mul r0.w, r3.w, r0.w
    max r1.w, r1.w, c4.x
    mul r0.xyz, r2.w, c30
    mul r0.w, r0.w, r1.w
    mad r1.xyz, r0, c5, r1
    mul r0.xyz, r0.w, c35
    mad oD0.xyz, r0, c5, r1
    mov oT0.xy, v3
    mov oD0.w, c5.w
