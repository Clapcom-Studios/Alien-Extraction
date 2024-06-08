using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

using YmirEngine;

public class Auto_Aim : YmirComponent
{
	public List<GameObject> enemies;
    public GameObject target;

	private GameObject playerObject;
    private Player player;

    private float angle;
	public void Start()
	{
        enemies = new List<GameObject>();
		target = null;

        playerObject = InternalCalls.GetGameObjectByName("Player");
        player = playerObject.GetComponent<Player>();

    }

    public void Update()
	{
        gameObject.SetPosition(player.gameObject.transform.globalPosition + (player.currentWeapon.gameObject.transform.GetForward() * 90f));
        gameObject.SetRotation(playerObject.transform.globalRotation * new Quaternion(0.7071f, 0.0f, 0.0f, -0.7071f)); // <- -90 Degree Quat

        target = null;
        SetTarget();

        player.isAiming = false;

        if (target != null)
		{
            Vector3 vec1 = Vector3.Normalize(gameObject.transform.globalPosition);
            Vector3 vec2 = Vector3.Normalize(target.transform.globalPosition);

            vec1.y = 0f;
            vec2.y = 0f;

            angle = (float)Math.Acos(Vector3.Dot(vec1, vec2));

            //Debug.Log("Target Pos: " + target.transform.globalPosition);
            //Debug.Log("Angle: " + angle);

            //playerObject.transform.LookAt(new Vector3(target.transform.globalPosition.x, playerObject.transform.globalPosition.y, target.transform.globalPosition.z));
            //player.target = target;

            player.isAiming = true;


        }

        enemies.Clear();
    }

    public void OnCollisionStay(GameObject other)
	{
        if (other.Tag == "Enemy")
		{
			if (!enemies.Contains(other)) { enemies.Add(other); }
		}
    }

    private void SetTarget()
	{
		float shortestDistance = 0f;

		for (int i = 0; i < enemies.Count; i++)
		{
			float distance = Vector3.Distance(gameObject.transform.globalPosition, enemies[i].transform.globalPosition);
			if (distance < shortestDistance || shortestDistance == 0f)
			{
                shortestDistance = distance;
				target = enemies[i];
            }
        }
	}

	public void AddEnemy(GameObject enemy)
	{
        if (!enemies.Contains(enemy)) { enemies.Add(enemy); }
    }
}