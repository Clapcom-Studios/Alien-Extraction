using System;
using System.Collections.Generic;
//using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class BH_Shotgun : YmirComponent
{
    GameObject playerObject;
    Player player;
    public void Start()
    {
        playerObject = InternalCalls.GetGameObjectByName("Player");
        player = playerObject.GetComponent<Player>();
    }

    public void Update()
    {
        gameObject.SetPosition(player.currentWeapon.gameObject.transform.globalPosition + player.currentWeapon.offset + (player.currentWeapon.gameObject.transform.GetForward() * player.currentWeapon.range * 0.5f));
        gameObject.SetRotation(playerObject.transform.globalRotation * new Quaternion(0.7071f, 0.0f, 0.0f, -0.7071f)); // <- -90 Degree Quat

        InternalCalls.Destroy(gameObject);
    }

    public void OnCollisionEnter(GameObject other)
    {

        if (other.Tag != "Enemy")
        {
            Audio.PlayAudio(gameObject, "W_FSADSurf");
        }
        else
        {
            Audio.PlayAudio(gameObject, "W_FSADEnemy");
            FaceHuggerBaseScript aux = other.GetComponent<FaceHuggerBaseScript>();

            if (aux != null)
            {
                GameObject FaceHuggerDamageParticles = InternalCalls.GetChildrenByName(aux.gameObject, "ParticlesDamageFaceHugger");
                if (FaceHuggerDamageParticles != null) Particles.PlayParticlesTrigger(FaceHuggerDamageParticles);
                aux.TakeDmg(player.currentWeapon.damage);
            }

            DroneXenomorphBaseScript aux2 = other.GetComponent<DroneXenomorphBaseScript>();
            if (aux2 != null)
            {
                GameObject DroneDamageParticles = InternalCalls.GetChildrenByName(aux2.gameObject, "ParticlesDamageDrone");
                if (DroneDamageParticles != null) Particles.PlayParticlesTrigger(DroneDamageParticles);
                aux2.TakeDmg(player.currentWeapon.damage);
            }

            QueenXenomorphBaseScript aux3 = other.GetComponent<QueenXenomorphBaseScript>();
            if (aux3 != null)
            {
                GameObject QueenDamageParticles = InternalCalls.GetChildrenByName(aux3.gameObject, "ParticlesDamageQueen");
                if (QueenDamageParticles != null) Particles.PlayParticlesTrigger(QueenDamageParticles);
                aux3.TakeDmg(player.currentWeapon.damage);
            }

            SpitterBaseScript aux4 = other.GetComponent<SpitterBaseScript>();
            if (aux4 != null)
            {
                GameObject SpitterDamageParticles = InternalCalls.GetChildrenByName(aux4.gameObject, "ParticlesDamageSpitter");
                if (SpitterDamageParticles != null) Particles.PlayParticlesTrigger(SpitterDamageParticles);
                aux4.TakeDmg(player.currentWeapon.damage);
            }

            Debug.Log("[ERROR] HIT ENEMy");
           
        }
    }
}