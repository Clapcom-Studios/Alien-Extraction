using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Shotgun : Weapon
{
    public int ammoInChamber;
    public float dispersion;
    public Shotgun() : base(WEAPON_TYPE.SHOTGUN) { }

    public override void Start()
    {
        playerObject = InternalCalls.GetGameObjectByName("Player");
        player = playerObject.GetComponent<Player>();

        reloadTime = 1.918f;

        switch (_upgrade)
        {
            case UPGRADE.LVL_0:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesShotgunDefault");
                ammo = 4; //16
                ammoInChamber = 2;
                fireRate = 1.3f;
                damage = 110; //55
                range = 32f; //10.5
                dispersion = range * 0.5f; //100º
                reloadTime = 1.918f;

                break;
            case UPGRADE.LVL_1:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesShotgunLVL1");
                ammo = 8; //26
                ammoInChamber = 2;
                fireRate = 1.2f;
                damage = 140; //70
                range = 32f; //10.5
                dispersion = range * 0.5f; //100º;
                reloadTime = 1.818f;

                break;
            case UPGRADE.LVL_2:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesShotgunLVL2");
                ammo = 10; //26
                ammoInChamber = 2;
                fireRate = 1.2f;
                damage = 160; //75
                range = 42f; //21f
                dispersion = range * 0.35f; //80º
                reloadTime = 1.618f;

                break;
            case UPGRADE.LVL_3_ALPHA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesShotgunLVL3A");
                ammo = 16; //28
                ammoInChamber = 2;
                fireRate = 0.7f;
                damage = 180; //80
                range = 42f;
                dispersion = range * 0.35f; //80º
                reloadTime = 0.918f;

                break;
            case UPGRADE.LVL_3_BETA:

                particlesGO = InternalCalls.GetChildrenByName(gameObject, "ParticlesShotgunLVL3B");
                ammo = 12; //28
                ammoInChamber = 4;
                fireRate = 1.0f;
                damage = 400; //80
                range = 42f;
                dispersion = range * 0.35f; //80º
                reloadTime = 1.118f;

                break;
            default:
                break;
        }

        currentAmmo = ammo;
    }

    public override void Shoot()
    {
        currentAmmo -= ammoInChamber;
        fireRateTimer = fireRate;

       

        Quaternion rot = gameObject.transform.globalRotation * new Quaternion(0.7071f, 0.0f, 0.0f, -0.7071f); // <- -90º Degree Quat

        //Estos numeros estan hardcpded, canviarlos cuando cuadre
        InternalCalls.CreateShotgunSensor(gameObject.transform.globalPosition + offset + (gameObject.transform.GetForward() * range * 0.5f), rot, range, dispersion, gameObject.transform.GetRight());
        Debug.Log("Position cone origin is:" + (gameObject.transform.globalPosition + offset + (gameObject.transform.GetForward() * range * 0.5f)));
        Debug.Log("Foward offset would have been:" +  (gameObject.transform.GetForward() * range * 0.5f));

        float angleOfShootgun = 0;
        switch (_upgrade)
        {
            case UPGRADE.LVL_0:
                angleOfShootgun = 0.872665f; //100º a radianes / 2 porque es a cada lado
                break;
            case UPGRADE.LVL_1:
                angleOfShootgun = 0.872665f; //100º a radianes / 2 porque es a cada lado
                break;
            case UPGRADE.LVL_2:
                angleOfShootgun = 0.698132f; //80º a radianes / 2 porque es a cada lado
                break;
            case UPGRADE.LVL_3_ALPHA:
                angleOfShootgun = 0.698132f; //80º a radianes / 2 porque es a cada lado
                break;
            case UPGRADE.LVL_3_BETA:
                angleOfShootgun = 0.698132f; //80º a radianes / 2 porque es a cada lado
                break;
            default:
                break;
        }
        
        Audio.PlayAudio(gameObject, "W_FSADShot");
        Particles.ParticleShoot(particlesGO, gameObject.transform.GetForward(), angleOfShootgun);
        Particles.PlayParticlesTrigger(particlesGO);
    }
    public override void Reload()
    {
        currentAmmo = ammo;

        //Audio.PlayAudio(gameObject, "W_FSADReload");
    }

    public override void StartReload()
    {
        reloading = true;
        reloadTimer = reloadTime;

        Audio.PlayAudio(gameObject, "W_FSADReload");
    }
}