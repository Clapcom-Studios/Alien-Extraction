using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Alien_Head_Trap : YmirComponent
{
	private GameObject _head = null;
	private GameObject _tongue = null;
	private GameObject particles = null;

	private float time = 0;
	private float animationTime = 2;
    public float fraction = 0f;
    private float startTime = 0;

    private Vector3 initialTonguePosition;
    private GameObject endPosition = null;

	public float damage;

    public void Start()
	{
		damage = 600f;

		_head = InternalCalls.CS_GetChild(gameObject, 0);
		_tongue = InternalCalls.CS_GetChild(gameObject, 1);
		endPosition = InternalCalls.CS_GetChild(gameObject, 2);
		particles = InternalCalls.CS_GetChild(gameObject,3);

		if (_tongue != null )
		{
            initialTonguePosition = _tongue.transform.localPosition;
        }
    }

	public void Update()
	{
		if (_head != null && _tongue != null)
		{
			time -= Time.deltaTime;
            fraction = (Time.time - startTime) / animationTime;
            fraction = Mathf.Clamp01(fraction);

            if (time > animationTime/2)	//From 2s to 1s
			{
                //Mover _tonge hacia delante (GetForward) durante este tiempo hasta un límite
                _tongue.transform.localPosition = Vector3.Lerp(initialTonguePosition, endPosition.transform.localPosition, fraction);
            }
			else if (time < animationTime/2 && time > 0)	//From 1s to 0s
			{
                //Mover _tonge hacia atrás (GetForward) durante este tiempo hasta la posición inicial
                _tongue.transform.localPosition = Vector3.Lerp(endPosition.transform.localPosition, initialTonguePosition, fraction);
            }
            else if (time < 0)	//From 0s to -inf
            {
				
            }
        }

	}

    public void OnCollisionEnter(GameObject other)
	{
		if (other.Tag == "Player" && time <= 0)
		{
			Particles.PlayParticles(particles);
			time = animationTime;
			startTime = Time.time;
		}
        if (other.Tag == "Player" && time > 0)
		{
			//TODO: Add SFX audio here
            other.GetComponent<Health>().TakeDmg(damage);
        }
            
    }
}