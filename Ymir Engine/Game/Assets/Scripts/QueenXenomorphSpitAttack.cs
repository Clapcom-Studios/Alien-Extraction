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
    private GameObject particleGo = null;
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
            //Quaternion rotation;
            //float angle = 0.0f;
            //Vector3 pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateQueenShrapnel(gameObject.transform.globalPosition, rotation);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 45.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 90.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 135.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 180.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 225.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 270.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);
            //pos = gameObject.transform.globalPosition;
            //pos.y += 2f;
            //angle = 315.0f;
            //rotation = Quaternion.RotateQuaternionY(gameObject.transform.globalRotation, angle);
            //InternalCalls.CreateSpitterAcidShrapnel(gameObject.transform.globalPosition, rotation, 150f);

            Vector3 pos = gameObject.transform.globalPosition;
            pos.y = 4f;
            //InternalCalls.CreateQueenPuddle(pos, gameObject.transform.globalRotation);
            InternalCalls.CreateGOFromPrefab("Assets/Prefabs", "Projectile-BossPuddle", pos);


            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && destroyed == false && player.GetComponent<Player>().vulnerable)
        {
            Vector3 distance = gameObject.transform.globalPosition - boss.transform.globalPosition;
            distance.y = boss.transform.globalPosition.y;

            healthScript.TakeDmg(bossScript.acidDMG);
            particleGo = InternalCalls.GetGameObjectByName("ParticlesAcidicBoss");
            if(particleGo != null) 
            {
                Particles.SetMaxDistance(particleGo,0.5f);
                
                Particles.SetEmittersPosition(particleGo, distance,1);
            }

            particleGo = InternalCalls.GetGameObjectByName("ParticlesPuddleBoss");



            if (particleGo != null) { Particles.SetEmittersPosition(particleGo, distance); }

            Particles.ParticlesForward(particleGo, gameObject.transform.GetForward(), 0, 0);
            Particles.ParticlesForward(particleGo, gameObject.transform.GetForward(), 1, 0);
            Particles.PlayParticlesTrigger(particleGo);

            destroyed = true;
        }
    }
}