using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class FaceHuggerAttack : YmirComponent
{
    public GameObject thisReference = null;

    public GameObject facehugger;

    private float damageTimer;

    private float attackDamage;

    private GameObject player;

    private Health healthScript;

    public void Start()
	{
        damageTimer = 0f;
        attackDamage = 1f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();

    }

    public void Update()
	{
        damageTimer -= Time.deltaTime;

        gameObject.SetRotation(facehugger.transform.globalRotation);

        //Since there's a small bug before attacking which causes the sensor not to move, the conditional is done this way for now
        if (facehugger.GetComponent<FaceHuggerBaseScript>().attackTimer > 0.5f)
        {
            gameObject.SetPosition(facehugger.transform.globalPosition);
        }
        else if (facehugger.GetComponent<FaceHuggerBaseScript>().attackTimer < 0.5f)
        {
            //gameObject.SetPosition(new Vector3(gameObject.transform.globalPosition.x, gameObject.transform.globalPosition.y, gameObject.transform.GetForward().z + 5));
            gameObject.SetVelocity(gameObject.transform.GetForward() * 50f);
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && damageTimer <= 0)
        {
            Debug.Log("[ERROR] HIT");
            damageTimer = 1.5f;
            healthScript.TakeDmg(attackDamage);
        }
    }

    //public void OnCollisionEnter(GameObject other)
    //{
    //    if (other.Name == "Player")
    //    {
    //        Debug.Log("[ERROR] HIT");
    //        damageTimer = 1.5f;
    //        healthScript.TakeDmg(attackDamage);
    //    }
    //}

}