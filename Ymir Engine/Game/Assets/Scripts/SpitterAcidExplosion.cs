using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class SpitterAcidExplosion : YmirComponent
{
    public GameObject thisReference = null;

    private float movementSpeed;

    private float damage = 300f; //350f

    private GameObject player;

    private Health healthScript;

    private bool destroyed;

    private float destroyTimer;

    private bool impulseDone = false;

    private float mass;

    Vector3 direction;

    public void Start()
    {
        movementSpeed = 4000f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        destroyed = false;
        destroyTimer = 0f;
        direction = gameObject.transform.globalPosition - player.transform.globalPosition;
        Quaternion rotation = Quaternion.LookRotation(direction);
        gameObject.SetRotation(rotation);
        damage = gameObject.GetMass();
        mass = gameObject.GetMass();
        //SET DAMAGE HERE
        switch (damage)
        {
            case 350f:
                damage = 100f;
                break;
            case 380f:
                damage = 120f;
                break;
            case 400f:
                damage = 130f;
                break;
        }
        gameObject.SetMass(1.0f);
    }

    public void Update()
    {
        if (impulseDone == false)
        {
            gameObject.SetImpulse(direction.normalized * -movementSpeed * mass * Time.deltaTime + new Vector3(0, 0.1f, 0));
            impulseDone = true; ;
        }

        destroyTimer += Time.deltaTime;

        if (destroyed)
        {
            InternalCalls.Destroy(gameObject);
        }
        else if (destroyTimer >= 1.3f)
        {
            //DO EXPLOSION
            Quaternion rotation;
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation);
            float angle = 45.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, damage);
            //angle = 90.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation);
            angle = 135.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, damage);
            //angle = 180.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation);
            angle = 225.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, damage);
            //angle = 270.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation);
            angle = 315.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, damage);
            //InternalCalls.CreateGOFromPrefab("Assets/Prefabs", "Projectile-SpitterShrapnelExplosion", gameObject.transform.globalPosition);

            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && destroyed == false && player.GetComponent<Player>().vulnerable)
        {
            healthScript.TakeDmg(damage);
            destroyed = true;
        }
        else if (other.Tag == "World")
        {
            destroyed = true;
        }
    }
}