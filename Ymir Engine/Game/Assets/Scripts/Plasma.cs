using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Plasma : Weapon
{
    public float damageEscalation;

    public float currentDamage;

    public Plasma() : base(WEAPON_TYPE.PLASMA) { }

    public override void Start()
    {
        playerObject = InternalCalls.GetGameObjectByName("Player");
        player = playerObject.GetComponent<Player>();

        range = 200;
        reloadTime = 3.162f;

        switch (_upgrade)
        {
            case UPGRADE.LVL_0:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesPlasmaDefault");
                ammo = 100;
                fireRate = 0.03f;
                damage = 2.4f;
                damageEscalation = 0.006f;

                break;
            case UPGRADE.LVL_1:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesPlasmaLVL1");
                ammo = 100;
                fireRate = 0.03f;
                damage = 3;
                damageEscalation = 0.01f;

                break;
            case UPGRADE.LVL_2:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesPlasmaLVL2");
                ammo = 200;
                fireRate = 0.02f;
                damage = 3.6f;
                damageEscalation = 0.01f;

                break;
            case UPGRADE.LVL_3_ALPHA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesPlasmaLVL3A");
                ammo = 200;
                fireRate = 0.015f;
                damage = 6f;
                damageEscalation = 0.018f;

                break;
            case UPGRADE.LVL_3_BETA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesPlasmaLVL3B");
                ammo = 350;
                fireRate = 0.02f;
                damage = 4.4f;
                damageEscalation = 0.01f;
                reloadTime = 2f;

                break;
            default:
                break;
        }

        currentDamage = damage;
        currentAmmo = ammo;
    } 
    public override void Shoot()
    {
        currentAmmo--;
        fireRateTimer = fireRate;

        Audio.PlayAudio(gameObject, "W_PlasmaShot");
        Particles.ParticleShoot(particlesGO, gameObject.transform.GetForward());
        float distanceParticles = gameObject.RaycastLenght(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
        if (distanceParticles == 0) { distanceParticles = range; } //Si no impacta con nada rango maximo
        else if (distanceParticles < range) { distanceParticles *= 0.8f; }
        Particles.SetMaxDistance(particlesGO, distanceParticles);
        Debug.Log("Distancia particles es:" + distanceParticles);
        Particles.PlayParticlesTrigger(particlesGO);

        GameObject target = null;

        switch (_upgrade)
        {
            case UPGRADE.LVL_0:
                target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
                break;
            case UPGRADE.LVL_1:
                target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
                break;
            case UPGRADE.LVL_2:
                target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
                break;
            case UPGRADE.LVL_3_ALPHA:
                target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
                break;
            case UPGRADE.LVL_3_BETA:
                // NEED TO ADD Piercing
                target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);
                break;
            default:
                break;
        }

        if (target != null)
        {
            if (target.Tag != "Enemy")
            {
                Audio.PlayAudio(gameObject, "W_PlasmaSurf");
            }
            else
            {
                Audio.PlayAudio(gameObject, "W_PlasmaEnemy");

                //---------------Xiao: Gurrada Pendiente de Cambiar----------------------------
                FaceHuggerBaseScript aux = target.GetComponent<FaceHuggerBaseScript>();

                if (aux != null)
                {
                    GameObject FaceHuggerDamageParticles = InternalCalls.GetChildrenByName(aux.gameObject, "ParticlesDamageFaceHugger");
                    if (FaceHuggerDamageParticles != null) Particles.PlayParticlesTrigger(FaceHuggerDamageParticles);
                    aux.TakeDmg(currentDamage*3);
                }

                DroneXenomorphBaseScript aux2 = target.GetComponent<DroneXenomorphBaseScript>();
                if (aux2 != null)
                {
                    GameObject DroneDamageParticles = InternalCalls.GetChildrenByName(aux2.gameObject, "ParticlesDamageDrone");
                    if (DroneDamageParticles != null) Particles.PlayParticlesTrigger(DroneDamageParticles);
                    aux2.TakeDmg(currentDamage * 3);
                }

                QueenXenomorphBaseScript aux3 = target.GetComponent<QueenXenomorphBaseScript>();
                if (aux3 != null)
                {
                    GameObject QueenDamageParticles = InternalCalls.GetChildrenByName(aux3.gameObject, "ParticlesDamageQueen");
                    if (QueenDamageParticles != null) Particles.PlayParticlesTrigger(QueenDamageParticles);
                    aux3.TakeDmg(currentDamage * 3);
                }

                SpitterBaseScript aux4 = target.GetComponent<SpitterBaseScript>();
                if (aux4 != null)
                {
                    GameObject SpitterDamageParticles = InternalCalls.GetChildrenByName(aux4.gameObject, "ParticlesDamageSpitter");
                    if (SpitterDamageParticles != null) Particles.PlayParticlesTrigger(SpitterDamageParticles);
                    aux4.TakeDmg(currentDamage * 3);
                }

                Debug.Log("[ERROR] HIT ENEMy");
                //-----------------------------------------------------------------------------------
            }
        }

        currentDamage += currentDamage * damageEscalation;
    }
    public override void Reload()
    {
        currentAmmo = ammo;

        //Audio.PlayAudio(gameObject, "W_PlasmaReload");
    }

    public override void StartReload()
    {
        reloading = true;
        reloadTimer = reloadTime;

        Audio.PlayAudio(gameObject, "W_PlasmaReload");
    }

    public void ResetDamage()
    {
        currentDamage = damage;
    }
}