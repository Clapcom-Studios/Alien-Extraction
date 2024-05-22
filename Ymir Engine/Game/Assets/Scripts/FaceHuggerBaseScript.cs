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

    public GameObject canvas;

    private WanderState pausedState;


    protected Vector3 targetPosition = null;

    public bool PlayerDetected = false;


    private float AttackDistance = 15f;

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
    private float attackDuration = 1.2f;
    public bool attackSensor = false;

    private bool walkPlaying = false;
    private bool attackDone = false;


    //Audio
    private float CryTimer = 10f;

    public void Start()
    {
        wanderState = WanderState.REACHED;
        wanderDuration = 5f;
        wanderTimer = wanderDuration;
        player = InternalCalls.GetGameObjectByName("Player");
        healthScript = player.GetComponent<Health>();
        agent = gameObject.GetComponent<PathFinding>();
        healthBar = InternalCalls.GetHealtBarObject(gameObject);
        knockBackSpeed = 200;
        knockBackTimer = 0.2f;
        stopedDuration = 1f;
        detectionRadius = 80f;
        wanderRange = 100f;
        cumDuration = 2f;
        cumDuration2 = 5f;

        //Drop items
        keys = "Nombre:,Probabilidad:";
        path = "Assets/Loot Tables/facehugger_loot.csv";
        numFields = 2;
        spawnRange = 15;
        level = InternalCalls.GetCurrentMap();

        switch (level)
        {
            case 1:
                commonProb = 93.0f;
                rareProb = 5.0f;
                epicProb = 2.0f;
                break;
            case int i when (i == 2 || i == 3):
                commonProb = 93.0f;
                rareProb = 5.0f;
                epicProb = 2.0f;
                break;
            case int i when (i == 4 || i == 5):
                commonProb = 93.0f;
                rareProb = 5.0f;
                epicProb = 2.0f;
                break;
            default:
                commonProb = 93.0f;
                rareProb = 5.0f;
                epicProb = 2.0f;
                break;
        }

        attackTimer = attackDuration;


        cumTimer = cumDuration2;

        agent.stoppingDistance = 2f;
        agent.speed = 1500f;
        agent.angularSpeed = 10f;

        life = 100f;
        armor = 0f;

        rarity = random.Next(101);

        Debug.Log("[ERROR]: " + rarity);

        if (rarity >= 90)
        {
            rarity = 2;
        }
        else if (rarity >= 70)
        {
            rarity = 1;
        }
        else
        {
            rarity = 0;
        }

        //Enemy rarity stats
        if (rarity == 1)
        {
            life = 330; //450
            armor = 0.1f; //0.1f
            agent.speed = 1650f;
        }
        else if (rarity == 2)
        {
            life = 440; //600
            armor = 0.2f; //0.2f
            agent.speed = 1800f;
        }

        SetColor();

        // Animations

        Animation.SetLoop(gameObject, "Idle_Facehugger", true);
        Animation.SetLoop(gameObject, "IdleCombat_Facehugger", true);
        Animation.SetLoop(gameObject, "Walk_Facehugger", true);

        Animation.SetResetToZero(gameObject, "Death_Facehugger", false);

        Animation.AddBlendOption(gameObject, "", "Idle_Facehugger", 10f);
        Animation.AddBlendOption(gameObject, "", "IdleCombat_Facehugger", 10f);
        Animation.AddBlendOption(gameObject, "", "IdleCombat_Facehugger", 10f);
        Animation.AddBlendOption(gameObject, "", "Walk_Facehugger", 10f);
        Animation.AddBlendOption(gameObject, "", "TailAttack_Facehugger", 10f);
        Animation.AddBlendOption(gameObject, "", "Death_Facehugger", 10f);

        Animation.PlayAnimation(gameObject, "Idle_Facehugger");
    }

    public void Update()
    {
        //Debug.Log("Level --------- " + level);
        if (CheckPause() )
        {
            SetPause(true);
            paused = true;
            return;
        }
        else if (paused)
        {
            SetPause(false);
            paused = false;
        }
        //Debug.Log("[ERROR] CurrentaState: " + wanderState);

        if (wanderState != WanderState.DEATH) { isDeath(); }

        CryTimer += Time.deltaTime;
        cumTimer2 -= Time.deltaTime;

        if (cumTimer2 <= 0)
        {
            switch (wanderState)
            {
                case WanderState.PAUSED:
                    //Do nothing
                    break;
                case WanderState.DEATH:

                    timePassed += Time.deltaTime;

                    if (timePassed >= 1.2f)
                    {
                        Debug.Log("[ERROR] DEATH");
                        itemPos = gameObject.transform.globalPosition;
                        DropItem();
                        InternalCalls.Destroy(gameObject);
                    }
                    return;

                case WanderState.REACHED:
                    agent.CalculateRandomPath(gameObject.transform.globalPosition, wanderRange);
                    wanderTimer = wanderDuration;
                    //Debug.Log("[ERROR] Current State: REACHED");
                    targetPosition = agent.GetPointAt(agent.GetPathSize() - 1);

                    Animation.PlayAnimation(gameObject, "Walk_Facehugger");
                    walkPlaying = true;
                    wanderState = WanderState.GOING;
                    break;

                case WanderState.GOING:
                    LookAt(agent.GetDestination());

                    MoveToCalculatedPos(agent.speed);
                    //Debug.Log("[ERROR] Current State: GOING");

                    IsReached(gameObject.transform.globalPosition, targetPosition);
                    break;


                case WanderState.CHASING:

                    LookAt(agent.GetDestination());
                    //Debug.Log("[ERROR] Current State: CHASING");
                    agent.CalculatePath(gameObject.transform.globalPosition, player.transform.globalPosition);

                    MoveToCalculatedPos(agent.speed);
                    break;

                case WanderState.STOPED:
                    //Debug.Log("[ERROR] Current State: STOPED");
                    ProcessStopped();
                    break;

                case WanderState.HIT:


                    Proccescumdown();

                    break;

                case WanderState.KNOCKBACK:

                    KnockBack(knockBackSpeed);
                    timePassed += Time.deltaTime;

                    if (timePassed >= knockBackTimer)
                    {
                        Debug.Log("[ERROR] End KnockBack");
                        wanderState = WanderState.REACHED;
                        timePassed = 0f;
                    }
                    break;

                case WanderState.ATTACK:
                    LookAt(player.transform.globalPosition);
                    Attack();
                    break;
            }

            if (wanderState != WanderState.PAUSED)
            {
                //Check if player is alive before chasing
                if (wanderState != WanderState.ATTACK && healthScript.GetCurrentHealth() > 0)
                {
                    if (CheckDistance(player.transform.globalPosition, gameObject.transform.globalPosition, detectionRadius))
                    {
                        if (wanderState != WanderState.KNOCKBACK && wanderState != WanderState.HIT)
                        {
                            if (CryTimer >= 10)
                            {
                                Audio.PlayAudio(gameObject, "FH_Cry");
                                CryTimer = 0;
                            }
                            if (walkPlaying == false)
                            {
                                Animation.PlayAnimation(gameObject, "Walk_Facehugger");
                                walkPlaying = true;
                            }
                            wanderState = WanderState.CHASING;
                            player.GetComponent<Player>().SetCombatAudioState();

                        }
                        //Attack if in range
                        if (CheckDistance(player.transform.globalPosition, gameObject.transform.globalPosition, AttackDistance))
                        {
                            if (wanderState == WanderState.CHASING && wanderState != WanderState.ATTACK && wanderState != WanderState.KNOCKBACK)
                            {
                                //Debug.Log("[ERROR] ATTACKING");
                                attackTimer = attackDuration;
                                gameObject.SetVelocity(gameObject.transform.GetForward() * 0);
                                Audio.PlayAudio(gameObject, "FH_Tail");
                                Animation.PlayAnimation(gameObject, "TailAttack_Facehugger");
                                walkPlaying = false;
                                wanderState = WanderState.ATTACK;
                            }
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
                Animation.PlayAnimation(gameObject, "Idle_Facehugger");
                walkPlaying = false;
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
                Animation.PlayAnimation(gameObject, "Idle_Facehugger");
                walkPlaying = false;
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
                attackSensor = true;
                attackTimer = attackDuration;

                stopedTimer = stopedDuration;
                wanderState = WanderState.STOPED;
                Animation.PlayAnimation(gameObject, "Idle_Facehugger");
                walkPlaying = false;
                attackDone = false;
            }
            else if (attackTimer <= 0.4f && attackDone == false)
            {
                Vector3 pos = gameObject.transform.globalPosition;
                pos.y += 10;
                pos.z -= 5;
                InternalCalls.CreateFaceHuggerTailAttack(pos, gameObject.transform.globalRotation);
                attackDone = true;
            }
        }
    }

    private void isDeath()
    {
        if (life <= 0)
        {
            Debug.Log("[ERROR] DEATH");
            gameObject.SetVelocity(new Vector3(0, 0, 0));
            Audio.PlayAudio(gameObject, "FH_Death");
            Animation.PlayAnimation(gameObject, "Death_Facehugger");
            wanderState = WanderState.DEATH;
            timePassed = 0;
        }
    }

    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Tail" && wanderState != WanderState.KNOCKBACK && wanderState != WanderState.DEATH)
        {
            Debug.Log("[ERROR] HIT!!");
            life -= 80;

            Animation.PlayAnimation(gameObject, "Idle_Facehugger");
            walkPlaying = false;
            wanderState = WanderState.KNOCKBACK;
        }
    }

    public void OnCollisionExit(GameObject other)
    {
        if (other.Tag == "Tail")
        {

        }
    }

    private void SetPause(bool pause)
    {
        if (pause && !paused)
        {
            pausedState = wanderState;
            wanderState = WanderState.PAUSED;
            Animation.PauseAnimation(gameObject);
            gameObject.SetVelocity(gameObject.transform.GetForward() * 0);
        }
        else if (wanderState == WanderState.PAUSED)
        {
            //If bool set to false when it was never paused, it will do nothing
            wanderState = pausedState;
            Debug.Log("[ERROR] state: " + wanderState);
            Animation.ResumeAnimation(gameObject);
        }
    }
}