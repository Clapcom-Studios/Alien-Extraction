using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;


enum EnemyState
{
    Idle,
    Moving,
    Attacking,
    Dead
}

public enum WanderState
{
    REACHED,
    GOING,
    CHASING,
    ATTACK,
    HIT,
    KNOCKBACK,
    DEATH,
    STOPED,
    PAUSED
}

public class Enemy : YmirComponent
{

    protected PathFinding agent;
    public GameObject player = null;

    public Health healthScript;
    public float movementSpeed;
    public float knockBackTimer;
    public float knockBackSpeed;

    protected WanderState wanderState;
    public float timePassed = 0f;
    public float life = 100f;

    //This may change depending on enemy rarity
    public float armor = 0;

    //0 = Common, 1 = Rare, 2 = Elite
    public int rarity = 0;

    public float wanderRange = 10f;

    public float detectionRadius = 60f;

    public float xSpeed = 0, ySpeed = 0;

    public bool paused = false;

    //Drop items
    protected string keys = " ";
    protected string path = " ";
    public int numFields;
    public int spawnRange;
    protected int level;
    protected float commonProb;
    protected float rareProb;
    protected float epicProb;

    protected Vector3 itemPos = Vector3.zero;
    public Random random = new Random();
    protected GameObject healthBar = null;


    public void TakeDmg(float dmg)
    {
        life -= (1 - armor) * dmg;
    }

    public void SetColor()
    {
        if (healthBar != null)
        {
            if (rarity == 0)
            {
                InternalCalls.SetColor(healthBar, new Vector3(0, 1, 0));
            }
            else if (rarity == 1)
            {
                InternalCalls.SetColor(healthBar, new Vector3(0, 0, 1));
            }
            else if (rarity == 2)
            {
                InternalCalls.SetColor(healthBar, new Vector3(0.502f, 0, 0.502f));
            }
        }
        else
        {
            Debug.Log("[WARNING] No HelthBar found !!!");
        }
    }

    public void LookAt(Vector3 pointToLook)
    {

        Vector3 direction = pointToLook - gameObject.transform.globalPosition;
        direction = direction.normalized;
        float angle = (float)Math.Atan2(direction.x, direction.z);

        //Debug.Log("Desired angle: " + (angle * Mathf.Rad2Deg).ToString());

        if (Math.Abs(angle * Mathf.Rad2Deg) < 1.0f)
            return;

        Quaternion dir = Quaternion.RotateAroundAxis(Vector3.up, angle);

        float rotationSpeed = Time.deltaTime * agent.angularSpeed;


        Quaternion desiredRotation = Quaternion.Slerp(gameObject.transform.localRotation, dir, rotationSpeed);

        gameObject.SetRotation(desiredRotation);
    }

    public void KnockBack(float speed)
    {

        Vector3 knockbackDirection = player.transform.globalPosition - gameObject.transform.globalPosition;
        knockbackDirection = knockbackDirection.normalized;
        knockbackDirection.y = 0f;
        gameObject.SetVelocity(knockbackDirection * -speed);

    }

    public bool CheckPause()
    {
        if (player.GetComponent<Player>().currentState == Player.STATE.STOP || player.GetComponent<Player>().currentState == Player.STATE.DEAD)
        {
            return true;
        }
        return false;
    }

    public void MoveToCalculatedPos(float speed)
    {
        Vector3 pos = gameObject.transform.globalPosition;
        Vector3 destination = agent.GetDestination();
        Vector3 direction = destination - pos;

        //So that enemies dont start going up
        direction.y = 0f;

        gameObject.SetVelocity(direction.normalized * speed * Time.deltaTime);
    }

    public bool CheckDistance(Vector3 first, Vector3 second, float checkRadius)
    {
        float deltaX = Math.Abs(first.x - second.x);
        float deltaY = Math.Abs(first.y - second.y);
        float deltaZ = Math.Abs(first.z - second.z);

        return deltaX <= checkRadius && deltaY <= checkRadius && deltaZ <= checkRadius;
    }
    public void DestroyEnemy()
    {
        Audio.PlayAudio(gameObject, "FH_Death");
        InternalCalls.Destroy(gameObject);
    }

    public void IsReached(Vector3 position, Vector3 destintion)
    {
        Vector3 roundedPosition = new Vector3(Mathf.Round(position.x),
                                                                    0,
                                                Mathf.Round(position.z));

        Vector3 roundedDestination = new Vector3(Mathf.Round(destintion.x),
                                                                            0,
                                                    Mathf.Round(destintion.z));
        Debug.Log("Position: " + position);
        Debug.Log("Rounded Position: " + roundedPosition);
        Debug.Log("Destination: " + destintion);
        Debug.Log("Rounded Destination: " + roundedDestination);
        float distanceX = roundedPosition.x - roundedDestination.x;
        float distanceZ = roundedPosition.z - roundedDestination.z;
        if (distanceX < 4 && distanceZ < 4)
        {
            wanderState = WanderState.REACHED;
            Debug.Log("Reached!!!!");

        }
    }

    public void DropItem()
    {
        string output = InternalCalls.CSVToStringKeys(path, keys);

        List<List<string>> result = DeconstructString(output, numFields);

        Debug.Log("Result:");
        foreach (var sublist in result)
        {
            Debug.Log("(" + string.Join(", ", sublist) + ")");

            // Check if sublist has at least two values
            if (sublist.Count >= 2)
            {
                // Extract the first two values
                string name = sublist[0];
                int probability;

                // Try parsing the probability value
                if (int.TryParse(sublist[1], out probability))
                {
                    // Call SpawnPrefab with extracted values
                    SpawnPrefab(name, probability);
                }
                else
                {
                    Debug.Log("[ERROR] Invalid probability value in sublist: " + string.Join(", ", sublist));
                }
            }
            else
            {
                Debug.Log("[ERROR] Sublist does not contain enough elements: " + string.Join(", ", sublist));
            }
        }
    }

    private static List<List<string>> DeconstructString(string input, int numberOfFields)
    {
        List<List<string>> output = new List<List<string>>();
        string[] parts = input.Split(';');

        List<string> currentList = new List<string>();

        for (int i = 0; i < parts.Length; i++)
        {
            currentList.Add(parts[i]);

            // Check if currentList has reached the desired number of fields
            if (currentList.Count == numberOfFields)
            {
                output.Add(currentList);
                currentList = new List<string>(); // Reset currentList for next set of fields
            }
        }

        // If there are any remaining elements in currentList, add them as a last incomplete sublist
        if (currentList.Count > 0)
        {
            output.Add(currentList);
        }

        return output;
    }

    protected void SpawnPrefab(string name, int probability)
    {
        int randNum = random.Next(0, 101);  //Generate a random number between 0 and 100
        Debug.Log("[WARNING] Rand Number: " + randNum);
        if (randNum <= probability)
        {
            //Spawn items in a range random position offset
            float randPosX = random.Next(-spawnRange, spawnRange + 1);
            float randPosZ = random.Next(-spawnRange, spawnRange + 1);
            Debug.Log("[WARNING] PickUp offset: " + randPosX + ", " + randPosZ);

            itemPos.x += randPosX;
            itemPos.z += randPosZ;

            int randNum2 = random.Next(0, 101);
            Debug.Log("[WARNING] Rand Number: " + randNum2);

            if(!name.Contains("resinheal") && !name.Contains("core_mythic"))
            {
                
                if (rarity == 0)    //Case common
                {
                    if (randNum2 < commonProb)
                    {
                        name += "_common";
                    }
                    else if (randNum2 < (commonProb + rareProb))
                    {
                        name += "_rare";
                    }
                    else
                    {
                        name += "_epic";
                    }
                }
                else if(rarity == 1)    //Case rare
                {
                    name += "_rare";
                }
                else if(rarity == 2)    //Case elite
                {
                    name += "_epic";
                }
                
            }
           
            Debug.Log("[WARNING] Name ---------- " + name);


            if (name.Contains("common"))
            {
                InternalCalls.CreateGOFromPrefab("Assets/Prefabs/Items/Common Items", name, itemPos);
            }
            else if (name.Contains("epic"))
            {
                InternalCalls.CreateGOFromPrefab("Assets/Prefabs/Items/Epic Items", name, itemPos);
            }
            else if (name.Contains("rare"))
            {
                InternalCalls.CreateGOFromPrefab("Assets/Prefabs/Items/Rare Items", name, itemPos);
            }
            else
            {
                // Case resin/core mythic
                InternalCalls.CreateGOFromPrefab("Assets/Prefabs/Items", name, itemPos);
            }

            //Clear the pos value
            itemPos = gameObject.transform.localPosition;
        }
    }


}

