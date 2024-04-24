using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;

using YmirEngine;



public class FaceHuggerBaseScript : Enemy
{
    public GameObject thisReference = null;


    protected Vector3 targetPosition = null;

    public bool PlayerDetected = false;

    
    private float AttackDistance = 20f;

    //private EnemyState state = EnemyState.Idle;
   

    private float wanderTimer;
    public float wanderDuration = 5f;

    private float stopedTimer;
    public float stopedDuration = 1f;

    private float cumTimer;
    public float cumDuration = 2f;

    private float cumTimer2;
    public float cumDuration2 = 5f;

    public float attackTimer;
    private float attackDuration = 0.8f;
    public bool attackSensor = false;

    //Audio
    private float CryTimer = 10f;

    public void Start()
    {
        wanderState = WanderState.REACHED;
        wanderDuration = 5f;
        wanderTimer = wanderDuration;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        //Debug.Log("[ERROR] Vida " + healthScript.currentHealth);
        agent = gameObject.GetComponent<PathFinding>();
        movementSpeed = 25f;
        stopedDuration = 1f;
        detectionRadius = 50f;
        wanderRange = 100f;
        cumDuration = 2f;
        cumDuration2 = 5f;

        attackTimer = attackDuration;


        cumTimer = cumDuration2;

        //Enemy rarity stats
        if (rarity == 1)
        {
            life = 350;
            armor = 0.1f;
            movementSpeed = 21.5f;
        }
        else if (rarity == 2)
        {
            life = 600;
            armor = 0.2f;
            movementSpeed = 23f;

        }

        agent.stoppingDistance = 2f;
        agent.speed = 50f;
        agent.angularSpeed = 10f;


    }

    public void Update()
    {
        //Debug.Log("[ERROR] Vida " + healthScript.currentHealth);
        CryTimer += Time.deltaTime;
        cumTimer2 -= Time.deltaTime;
        if (cumTimer2 <= 0)
        {
            switch (wanderState)
            {
                case WanderState.REACHED:
                    agent.CalculateRandomPath(gameObject.transform.globalPosition, wanderRange);
                    wanderTimer = wanderDuration;
                    //Debug.Log("[ERROR] Current State: REACHED");
                    targetPosition = agent.GetPointAt(agent.GetPathSize() - 1);
                    //Debug.Log("[ERROR] TargetPosition: " + targetPosition); 
                    wanderState = WanderState.GOING;
                    break;

                case WanderState.GOING:
                    LookAt(agent.GetDestination());
                    //Debug.Log("[ERROR] Speed" + agent.speed);
                    MoveToCalculatedPos(agent.speed);
                    Debug.Log("[ERROR] Current State: GOING");

                    IsReached(gameObject.transform.globalPosition, targetPosition);
                    break;


                case WanderState.CHASING:

                    LookAt(agent.GetDestination());
                    //Debug.Log("[ERROR] Current State: CHASING");
                    agent.CalculatePath(gameObject.transform.globalPosition, player.transform.globalPosition);

                    MoveToCalculatedPos(agent.speed);

                    Debug.Log("[ERROR] Current State: CHASING");
                    break;

                case WanderState.STOPED:
                    //Debug.Log("[ERROR] Current State: STOPED");
                    ProcessStopped();
                    break;

                case WanderState.HIT:

                    Proccescumdown();

                    break;

                case WanderState.ATTACK:
                    LookAt(player.transform.globalPosition);
                    Attack();
                    break;
            }

            ////Check if player is alive before chasing
            if (wanderState != WanderState.ATTACK && healthScript.GetCurrentHealth() > 0)
            {

                if (CheckDistance(player.transform.globalPosition, gameObject.transform.globalPosition, detectionRadius))
                {

                    if (wanderState != WanderState.HIT)
                    {
                        
                        if (CryTimer >= 10) 
                        {
                            Audio.PlayAudio(gameObject, "FH_Cry");
                            CryTimer = 0;
                        }
                        wanderState = WanderState.CHASING;
                        
                    }
                    //Attack if in range
                    if (CheckDistance(player.transform.globalPosition, gameObject.transform.globalPosition, AttackDistance))
                    {

                        if (wanderState == WanderState.CHASING && wanderState != WanderState.ATTACK)
                        {
                            //Debug.Log("[ERROR] ATTACKING");
                            attackTimer = attackDuration;
                            gameObject.SetVelocity(gameObject.transform.GetForward() * 0);
                            Audio.PlayAudio(gameObject, "FH_Tail");
                            wanderState = WanderState.ATTACK;
                        }
                    }

                }
            }


        }



    }



    private void Proccescumdown()
    {
        if (cumTimer > 0)
        {
            cumTimer -= Time.deltaTime;
            if (cumTimer <= 0)
            {
                //Debug.Log("[ERROR] Reached");
                wanderState = WanderState.REACHED;
            }
        }
    }

    private void ProcessStopped()
    {
        if (stopedTimer > 0)
        {
            stopedTimer -= Time.deltaTime;
            if (stopedTimer <= 0)
            {
                wanderState = WanderState.REACHED;
            }
        }
    }

    public WanderState GetState()
    {
        return wanderState;
    }
    private void Attack()
    {
        if (attackTimer > 0)
        {
            attackTimer -= Time.deltaTime;

            if (attackTimer <= 0)
            {
                ////IF HIT, Do damage
                //healthScript.TakeDmg(3);
                //Debug.Log("[ERROR] DID DAMAGE");
                attackSensor = true;
                attackTimer = attackDuration;
                
                stopedTimer = stopedDuration;
                wanderState = WanderState.STOPED;

            }
        }
    }



    public void OnCollisionStay(GameObject other)
    {
        //if (other.Name == "Player" && wanderState != WanderState.HIT)
        //{

        //    Debug.Log("[ERROR] Name: " + other.Name);
        //    Debug.Log("[ERROR] HIT!!!");
        //    gameObject.SetVelocity(gameObject.transform.GetForward() * 0);

        //    gameObject.SetImpulse(gameObject.transform.GetForward() * -10);
        //    healthScript.TakeDmg(3);
        //    wanderState = WanderState.HIT;
        //    cumTimer = cumDuration;
        //}
    }
}