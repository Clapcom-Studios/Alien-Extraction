using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class SMG : Weapon
{
    public SMG() : base(WEAPON_TYPE.SMG) {}

    public override void Start()
    {
        playerObject = InternalCalls.GetGameObjectByName("Player");
        player = playerObject.GetComponent<Player>();

        range = 100;
        reloadTime = 1.056f;

        switch (_upgrade)
        {
            case UPGRADE.LVL_0:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesSmgDefault");
                ammo = 35;
                fireRate = 0.07f;
                damage = 5;

                break;
            case UPGRADE.LVL_1:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesSmgLVL1");
                ammo = 35;
                fireRate = 0.06f;
                damage = 9;

                break;
            case UPGRADE.LVL_2:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesSmgLVL2");
                ammo = 40;
                fireRate = 0.05f;
                damage = 11;

                break;
            case UPGRADE.LVL_3_ALPHA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesSmgLVL3A");
                ammo = 110;
                fireRate = 0.04f;
                damage = 13;

                break;
            case UPGRADE.LVL_3_BETA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesSmgLVL3B");
                ammo = 40;
                fireRate = 0.02f;
                damage = 10;

                break;
            default:
                break;
        }

        currentAmmo = ammo;
    }

    public override void Shoot()
    {
        currentAmmo--;
        fireRateTimer = fireRate;

        Audio.PlayAudio(gameObject, "W_FirearmShot");
        Particles.ParticleShoot(particlesGO, gameObject.transform.GetForward());
        Particles.PlayParticlesTrigger(particlesGO);

        GameObject target;

        target = gameObject.RaycastHit(gameObject.transform.globalPosition + offset, gameObject.transform.GetForward(), range);

        if (target != null)
        {
            if (target.Tag != "Enemy")
            {
                Audio.PlayAudio(gameObject, "W_FirearmSurf");
            }
            else
            {
                Audio.PlayAudio(gameObject, "W_FirearmEnemy");

                //---------------Xiao: Gurrada Pendiente de Cambiar----------------------------
                FaceHuggerBaseScript aux = target.GetComponent<FaceHuggerBaseScript>();

                if (aux != null)
                {
                    GameObject FaceHuggerDamageParticles = InternalCalls.GetChildrenByName(aux.gameObject, "ParticlesDamageFaceHugger");
                    if(FaceHuggerDamageParticles != null) Particles.PlayParticlesTrigger(FaceHuggerDamageParticles);
                    aux.TakeDmg(damage);
                }

                DroneXenomorphBaseScript aux2 = target.GetComponent<DroneXenomorphBaseScript>();
                if (aux2 != null)
                {
                    GameObject DroneDamageParticles = InternalCalls.GetChildrenByName(aux2.gameObject, "ParticlesDamageDrone");
                    if(DroneDamageParticles != null) Particles.PlayParticlesTrigger(DroneDamageParticles);
                    aux2.TakeDmg(damage);
                }

                QueenXenomorphBaseScript aux3 = target.GetComponent<QueenXenomorphBaseScript>();
                if (aux3 != null)
                {
                    GameObject QueenDamageParticles = InternalCalls.GetChildrenByName(aux3.gameObject, "ParticlesDamageQueen");
                    if (QueenDamageParticles != null) Particles.PlayParticlesTrigger(QueenDamageParticles);
                    aux3.TakeDmg(damage);
                }

                SpitterBaseScript aux4 = target.GetComponent<SpitterBaseScript>();
                if (aux4 != null)
                {
                    GameObject SpitterDamageParticles = InternalCalls.GetChildrenByName(aux4.gameObject, "ParticlesDamageSpitter");
                    if (SpitterDamageParticles != null) Particles.PlayParticlesTrigger(SpitterDamageParticles);
                    aux4.TakeDmg(damage);
                }

                Debug.Log("[ERROR] HIT ENEMY");
                //-----------------------------------------------------------------------------------
            }
        }
    }

    public override void Reload()
    {
        currentAmmo = ammo;

        //Audio.PlayAudio(gameObject, "W_FirearmReload");
    }
    public override void StartReload()
    {
        reloading = true;
        reloadTimer = reloadTime;

        Audio.PlayAudio(gameObject, "W_FirearmReload");
    }
}