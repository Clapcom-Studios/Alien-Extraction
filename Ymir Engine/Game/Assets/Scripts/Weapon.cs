using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public enum WEAPON_TYPE
{
    NONE = -1,

    SMG,
    SHOTGUN,
    PLASMA
}

public enum UPGRADE
{
    NONE = -1,

    LVL_0,
    LVL_1,
    LVL_2,
    LVL_3_ALPHA,
    LVL_3_BETA,
}

public abstract class Weapon : YmirComponent
{
    private string _name;

    protected WEAPON_TYPE _type;
    protected UPGRADE _upgrade;

    public int ammo;
    public float fireRate;
    public float damage;
    public float reloadTime;
    public float range;

    public int currentAmmo;
    protected float fireRateTimer;
    protected float reloadTimer;
    protected bool reloading;

    public GameObject playerObject;
    protected Player player;

    public GameObject particlesGO;

    public Vector3 offset = null;
    public Weapon(WEAPON_TYPE type = WEAPON_TYPE.NONE, UPGRADE upgrade = UPGRADE.NONE)
    {
        _type = type;
        _upgrade = upgrade;

        _name = "";

        ammo = 0;
        fireRate = 0;
        damage = 0;
        reloadTime = 0;
        range = 0;

        currentAmmo = 0;
        fireRateTimer = 0;
        reloadTimer = 0;

       offset = new Vector3(0, 15, 0);
    }

    public string Name { get { return _name; } }
    public WEAPON_TYPE Type { 
        get { return _type; } 
        set { _type = value; }
    }
    public UPGRADE Upgrade { 
        get { return _upgrade; } 
        set { _upgrade = value; }
    }
    public abstract void Shoot();
    public abstract void Reload();

    public abstract void Start();
    public void Update()
    {
        if (fireRateTimer > 0) fireRateTimer -= Time.deltaTime;
        if (reloadTimer > 0 && reloading)
        {
            reloadTimer -= Time.deltaTime;
            if (reloadTimer <= 0 )
            {
                Reload();
                reloading = false;

                player.csBullets.UseBullets();
            }
        }
    }
    public bool ShootAvailable()
    {
        return (fireRateTimer <= 0 && currentAmmo > 0) ? true : false;
    }

    public bool ReloadAvailable()
    {
        return (reloadTimer <= 0 && currentAmmo < ammo) ? true : false;
    }

    public abstract void StartReload();
    public void InterruptReload()
    {
        reloading = false;
        reloadTimer = 0;

        Audio.StopAudio(gameObject);
    }
}