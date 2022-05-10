#include "lightfinder.h"

RwBool
FindAtomicLights(RpAtomic *atomic,
                 RwV4d *ambientLightConstant,
                 _rpD3D8VertexShaderLights  *lightsArray,
                 RwBool hasNormals)
{
    RwBool      haveLights;
    RwLLLink    *cur, *end;
    RpWorld     *world;

    RWFUNCTION(RWSTRING("FindAtomicLights"));

    haveLights = FALSE;

    /*
     * GLOBAL LIGHTS
     */
    world = (RpWorld *)RWSRCGLOBAL(curWorld);

    cur = rwLinkListGetFirstLLLink(&world->directionalLightList);
    end = rwLinkListGetTerminator(&world->directionalLightList);
    while (cur != end)
    {
        RpLight *light;

        light = rwLLLinkGetData(cur, RpLight, inWorld);

        /* NB light may actually be a dummyTie from a enclosing ForAll */
        if (light != NULL &&
            (rwObjectTestFlags(light, rpLIGHTLIGHTATOMICS)))
        {
            switch (RpLightGetType(light))
            {
                case rpLIGHTAMBIENT:
                    {
                        const RwRGBAReal    *color;

                        color = RpLightGetColor(light);

                        ambientLightConstant->x += color->red;
                        ambientLightConstant->y += color->green;
                        ambientLightConstant->z += color->blue;
                    }
                    break;

                case rpLIGHTDIRECTIONAL:
                    if (hasNormals)
                    {
                        if (lightsArray->numDirectional < MAX_LIGHTS)
                        {
                            lightsArray->directional[lightsArray->numDirectional] = light;

                            lightsArray->numDirectional += 1;

                            haveLights = TRUE;
                        }
                    }
                    break;

                default:
                    RWASSERT(0&&"unsupported light type");
            }
        }

        /* Next */
        cur = rwLLLinkGetNext(cur);
    } /* while */

    /*
     * POINT and SPOT lights
     */
    if (hasNormals)
    {
        /* don't light this atomic with the same light more than once! */
        RWSRCGLOBAL(lightFrame)++;

        /* For all sectors that this atomic lies in, apply all lights within */
        cur = rwLinkListGetFirstLLLink(&atomic->llWorldSectorsInAtomic);
        end = rwLinkListGetTerminator(&atomic->llWorldSectorsInAtomic);
        while (cur != end)
        {
            RpTie       *tpTie = rwLLLinkGetData(cur, RpTie, lWorldSectorInAtomic);
            RwLLLink    *curLight, *endLight;

            /* Lights in the sector */
            curLight = rwLinkListGetFirstLLLink(&tpTie->worldSector->lightsInWorldSector);
            endLight = rwLinkListGetTerminator(&tpTie->worldSector->lightsInWorldSector);

            while (curLight != endLight)
            {
                RpLightTie  *lightTie;
                RpLight     *light;

                lightTie = rwLLLinkGetData(curLight, RpLightTie, lightInWorldSector);

                light = lightTie->light;

                /* NB lightTie may actually be a dummyTie from a enclosing ForAll */

                /* Check to see if the light has already been applied and is set to
                * light atomics
                */
                if ( light != NULL &&
                    (rwObjectTestFlags(light, rpLIGHTLIGHTATOMICS)) &&
                    (light->lightFrame != RWSRCGLOBAL(lightFrame)))
                {
                    const RwMatrix  *matrixLight;
                    const RwV3d     *pos;
                    const RwSphere  *sphere;
                    RwV3d           distanceVector;
                    RwReal          distanceSquare;
                    RwReal          distanceCollision;

                    /* don't light this atomic with the same light again! */
                    light->lightFrame = RWSRCGLOBAL(lightFrame);

                    /* Does the light intersect the atomics bounding sphere */
                    matrixLight = RwFrameGetLTM(RpLightGetFrame(light));

                    pos = &(matrixLight->pos);

                    sphere = RpAtomicGetWorldBoundingSphere(atomic);

                    RwV3dSub(&distanceVector, &(sphere->center), pos);

                    distanceSquare = RwV3dDotProduct(&distanceVector, &distanceVector);

                    distanceCollision = (sphere->radius + RpLightGetRadius(light));

                    if (distanceSquare < (distanceCollision * distanceCollision))
                    {
                        switch (RpLightGetType(light))
                        {
                            case rpLIGHTPOINT:
                                if (lightsArray->numPoint < MAX_LIGHTS)
                                {
                                    lightsArray->point[lightsArray->numPoint] = light;

                                    lightsArray->numPoint += 1;

                                    haveLights = TRUE;
                                }
                                break;

                            case rpLIGHTSPOT:
                            case rpLIGHTSPOTSOFT:
                                if (lightsArray->numSpot < MAX_LIGHTS)
                                {
                                    lightsArray->spot[lightsArray->numSpot] = light;

                                    lightsArray->numSpot += 1;

                                    haveLights = TRUE;
                                }
                                break;

                            default:
                                break;
                        }
                    }
                }

                /* Next */
                curLight = rwLLLinkGetNext(curLight);
            }

            /* Next one */
            cur = rwLLLinkGetNext(cur);
        }
    }

    RWRETURN(haveLights);
}

RwBool
FindSectorLights(const RpWorldSector *worldSector,
                 RwV4d *ambientLightConstant,
                 _rpD3D8VertexShaderLights  *lightsArray)
{
    RwBool      haveLights;
    RpWorld     *world;
    RwLLLink    *cur, *end;
    const RwLLLink  *curLight, *endLight;

    RWFUNCTION(RWSTRING("FindSectorLights"));

    haveLights = FALSE;

    /*
     * GLOBAL LIGHTS
     */
    world = (RpWorld *)RWSRCGLOBAL(curWorld);

    cur = rwLinkListGetFirstLLLink(&world->directionalLightList);
    end = rwLinkListGetTerminator(&world->directionalLightList);
    while (cur != end)
    {
        RpLight *light;

        light = rwLLLinkGetData(cur, RpLight, inWorld);

        /* NB light may actually be a dummyTie from a enclosing ForAll */
        if (light != NULL &&
            (rwObjectTestFlags(light, rpLIGHTLIGHTWORLD)))
        {
            switch (RpLightGetType(light))
            {
                case rpLIGHTAMBIENT:
                    {
                        const RwRGBAReal    *color;

                        color = RpLightGetColor(light);

                        ambientLightConstant->x += color->red;
                        ambientLightConstant->y += color->green;
                        ambientLightConstant->z += color->blue;
                    }
                    break;

                case rpLIGHTDIRECTIONAL:
                    if (lightsArray->numDirectional < MAX_LIGHTS)
                    {
                        lightsArray->directional[lightsArray->numDirectional] = light;

                        lightsArray->numDirectional += 1;

                        haveLights = TRUE;
                    }
                    break;

                default:
                    RWASSERT(0&&"unsupported light type");
            }
        }

        /* Next */
        cur = rwLLLinkGetNext(cur);
    } /* while */

    /* don't light this sector with the same light more than once! */
    RWSRCGLOBAL(lightFrame)++;

    /*
     * POINT and SPOT lights
     */
    /* Lights in the sector */
    curLight = rwLinkListGetFirstLLLink(&worldSector->lightsInWorldSector);
    endLight = rwLinkListGetTerminator(&worldSector->lightsInWorldSector);
    while (curLight != endLight)
    {
        RpLightTie  *lightTie;
        RpLight     *light;

        lightTie = rwLLLinkGetData(curLight, RpLightTie, lightInWorldSector);

        light = lightTie->light;

        /* NB lightTie may actually be a dummyTie from a enclosing ForAll */

        /* Check to see if the light has already been applied and is set to
         * light sectors
         */
        if (light != NULL &&
            (rwObjectTestFlags(light, rpLIGHTLIGHTWORLD)) &&
            (light->lightFrame != RWSRCGLOBAL(lightFrame)))
        {
            /* don't light this sector with the same light again! */
            light->lightFrame = RWSRCGLOBAL(lightFrame);

            switch (RpLightGetType(light))
            {
                case rpLIGHTPOINT:
                    if (lightsArray->numPoint < MAX_LIGHTS)
                    {
                        lightsArray->point[lightsArray->numPoint] = light;

                        lightsArray->numPoint += 1;

                        haveLights = TRUE;
                    }
                    break;

                case rpLIGHTSPOT:
                case rpLIGHTSPOTSOFT:
                    if (lightsArray->numSpot < MAX_LIGHTS)
                    {
                        lightsArray->spot[lightsArray->numSpot] = light;

                        lightsArray->numSpot += 1;

                        haveLights = TRUE;
                    }
                    break;

                default:
                    break;
            }
        }

        /* Next */
        curLight = rwLLLinkGetNext(curLight);
    }

    RWRETURN(haveLights);
}
