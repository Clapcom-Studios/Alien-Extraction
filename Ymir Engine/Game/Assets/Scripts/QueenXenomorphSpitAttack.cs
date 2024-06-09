using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class QueenXenomorphSpitAttack : YmirComponent
{
    public GameObject thisReference = null;

    private float movementSpeed;

    private float damage = 600f;

    private GameObject player;

    private GameObject boss;
    private QueenXenomorphBaseScript bossScript;

    private Health healthScript;

    private bool destroyed;

    private float destroyTimer;

    public void Start()
    {
        movementSpeed = 7000f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        boss = InternalCalls.GetGameObjectByName("Boss");
        bossScript = boss.GetComponent<QueenXenomorphBaseScript>();
        Vector3 impulse = gameObject.transform.GetForward();
        impulse += new Vector3(0, -0.02f, 0);
        gameObject.SetImpulse(impulse * movementSpeed * Time.deltaTime);
        destroyed = false;
        destroyTimer = 0f;
    }

    public void Update()
    {
        destroyTimer += Time.deltaTime;

        if (destroyed || destroyTimer >= 1.2f)
        {
            //DO EXPLOSION
            Quaternion rotation;
            float angle = 0.0f;
            Vector3 pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateQueenShrapnel(gameObject.transform.globalPosition, rotation);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 45.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 90.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 135.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 180.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 225.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 270.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            pos = gameObject.transform.globalPosition;
            pos.y += 2f;
            angle = 315.0f;
            rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);

            pos = gameObject.transform.globalPosition;
            pos.y = 0f;
            InternalCalls.CreateQueenPuddle(pos, gameObject.transform.globalRotation);


            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && destroyed == false && player.GetComponent<Player>().vulnerable)
        {
            healthScript.TakeDmg(bossScript.acidDMG);
            destroyed = true;
        }
    }
}