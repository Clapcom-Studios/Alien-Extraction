using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class DroneXenomorphAttack : YmirComponent
{

    public GameObject thisReference = null;

    public GameObject drone;

    private float damageTimer;

    private float clawDamage;
    private float tailDamage;
    private float attackSpeed;

    private float attackRange;

    private GameObject player;

    private Health healthScript;

    public void Start()
	{
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        damageTimer = 0f;
        clawDamage = 200f;
        tailDamage = 250f;
        attackRange = 100f;
        attackSpeed = 1f;
    }

    public void Update()
	{
        damageTimer -= Time.deltaTime;

        gameObject.SetRotation(drone.transform.globalRotation);

        if (drone.GetComponent<DroneXenomorphBaseScript>().GetState() != DroneState.CLAW && drone.GetComponent<DroneXenomorphBaseScript>().GetState() != DroneState.TAIL)
        {
            gameObject.SetPosition(drone.transform.globalPosition);
            gameObject.SetVelocity(drone.transform.GetForward() * 0f * Time.deltaTime);
        }
        else if (drone.GetComponent<DroneXenomorphBaseScript>().GetState() == DroneState.CLAW && drone.GetComponent<DroneXenomorphBaseScript>().timeCounter > 0.6f)
        {
            attackSpeed = 0.8f;

            if (drone.GetComponent<DroneXenomorphBaseScript>().CheckDistance(gameObject.transform.globalPosition, drone.transform.globalPosition, attackRange))
            {
                gameObject.SetVelocity(drone.transform.GetForward() * 3000f * attackSpeed * Time.deltaTime);
            }
            else
            {
                gameObject.SetVelocity(drone.transform.GetForward() * 0f * Time.deltaTime);
            }
        }
        else if (drone.GetComponent<DroneXenomorphBaseScript>().GetState() == DroneState.TAIL && drone.GetComponent<DroneXenomorphBaseScript>().timeCounter > 0.35f)
        {
            attackSpeed = 1.8f;

            if (drone.GetComponent<DroneXenomorphBaseScript>().CheckDistance(gameObject.transform.globalPosition, drone.transform.globalPosition, attackRange) && drone.GetComponent<DroneXenomorphBaseScript>().timeCounter < 0.6f)
            {
                gameObject.SetVelocity(drone.transform.GetForward() * 3000f * attackSpeed * Time.deltaTime);
            }
            else
            {
                gameObject.SetVelocity(drone.transform.GetForward() * 0f * Time.deltaTime);
            }
        }

    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Name == "Player" && damageTimer <= 0)
        {
            //Debug.Log("[ERROR] HIT");
            damageTimer = 0.8f;
            if (drone.GetComponent<DroneXenomorphBaseScript>().GetState() == DroneState.CLAW)
            {
                healthScript.TakeDmg(clawDamage);
            }
            else if (drone.GetComponent<DroneXenomorphBaseScript>().GetState() == DroneState.TAIL)
            {
                healthScript.TakeDmg(tailDamage);
            }
        }
    }

}