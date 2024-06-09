using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class SpitterAcidSpit : YmirComponent
{
    public GameObject thisReference = null;

    private float movementSpeed;

	private float damage = 250f;

    private GameObject player;

    private Health healthScript;

    private bool destroyed;

    private float destroyTimer;

    private bool impulseDone = false;

    private float mass;

    Vector3 direction;

    public void Start()
	{
		movementSpeed = 7000f;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        destroyed = false;
        destroyTimer = 0f;
        direction = gameObject.transform.globalPosition - player.transform.globalPosition;
        Quaternion rotation = Quaternion.LookRotation(direction);
        gameObject.SetRotation(rotation);
        damage = gameObject.GetMass();
        mass = gameObject.GetMass();
        gameObject.SetMass(1.0f);
    }

    public void Update()
	{
        if (impulseDone == false)
        {
            gameObject.SetImpulse(direction.normalized * -movementSpeed * mass * Time.deltaTime + new Vector3(0,0.1f,0));
            impulseDone = true; ;
        }

        destroyTimer += Time.deltaTime;

        if (destroyed || destroyTimer >= 2f) 
        {
            InternalCalls.Destroy(gameObject);
        }

    }

    public void OnCollisionEnter(GameObject other)
    {
        Debug.Log("[ERROR] :" + other.Name);
        Debug.Log("[ERROR] :" + other.Tag);

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