﻿using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;



public class Core : YmirComponent
{
    public static Core instance;
    
    enum STATE : int
    {
        NONE = -1,
        IDLE,
        MOVE,
        DASH,
        SHOOT
    }
    enum INPUT : int
    {
        IN_IDLE,
        IN_MOVE,
        IN_DASH,
        IN_DASH_END,
        IN_SHOOTING,
        IN_SHOOTING_END,
        IN_SHOOT,
        IN_SHOOT_END,
        IN_SEC_SHOOT,
        IN_DEAD
    }

    private STATE currentState = STATE.NONE;
    private List<INPUT> inputList = new List<INPUT>();
    // Movment
    public float movementSpeed = 20f;
    private double angle = 0.0f;
    
    //Controller Variables

    Vector3 gamepadInput;

    public GameObject reference = null;
	public GameObject turret = null;
	public GameObject shootPoint = null;
		
	
    public float mouseSens = 5.0f;

	public bool testBool = true;
	public string testString = "Juan";
    public bool start = true;

    public Vector3 testOtherClass; //Should find a way to tell if the class is a gameobject or not

	public void Update(/*int x*/)
	{
        
        //Hardcoceado para que sea una especia de "Start()"
        if ( start )
        {
            Debug.Log("[WARNING] testString: " + testString);
            reference = InternalCalls.GetGameObjectByName("Test1");
            reference.name = InternalCalls.GetGameObjectByName("Test1").Name;

            Debug.Log("[WARNING] Reference str: "  + reference.name);
            Debug.Log("juan " + testString);
           

            start = false;
         
            Debug.Log("[WARNING] START");

            //testString = reference.Tag;
            testString = "Update string";

        }

        float z = gameObject.transform.localPosition.x;
        Debug.Log("[WARNING] Position Z: " + z.ToString());

        if (this.reference == null)
        {
            Debug.Log("[ERROR] Reference on Core.cs was 'null'");
            return;
        }
        float x = reference.transform.localPosition.x;
        Debug.Log("[WARNING] Position: " + x.ToString());


        if (Input.GetKey(YmirKeyCode.W) == KeyState.KEY_REPEAT)
            reference.transform.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
        if (Input.GetKey(YmirKeyCode.S) == KeyState.KEY_REPEAT)
            reference.transform.localPosition += reference.GetForward() * -movementSpeed * Time.deltaTime;
        //if (Input.GetKey(YmirKeyCode.A) == KeyState.KEY_REPEAT)
        //    reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, rotationSpeed * Time.deltaTime);
        //if (Input.GetKey(YmirKeyCode.D) == KeyState.KEY_REPEAT)
        //    reference.localRotation *= Quaternion.RotateAroundAxis(Vector3.up, -rotationSpeed * Time.deltaTime);

        //Destroy current GameObject - It works
        if (Input.GetKey(YmirKeyCode.X) == KeyState.KEY_REPEAT)
            InternalCalls.Destroy(reference);

        //Create a GameObject - Not working
        if (Input.GetKey(YmirKeyCode.C) == KeyState.KEY_REPEAT)
            InternalCalls.CreateGameObject("Cube", reference);




        //if (Input.GetMouseX() != 0 && turret != null)
        //    turret.localRotation = Quaternion.RotateAroundAxis(Vector3.up, -Input.GetMouseX() * mouseSens * Time.deltaTime) * turret.localRotation;

        ////if (Input.GetMouseY() != 0 && turret != null)
        ////    turret.localRotation = turret.localRotation * Quaternion.RotateAroundAxis(Vector3.right, -Input.GetMouseY() * Time.deltaTime);

        //if (Input.GetMouseClick(MouseButton.LEFT) == KeyState.KEY_DOWN)
        //{
        //    InternalCalls.CreateBullet(shootPoint.globalPosition, shootPoint.globalRotation, shootPoint.globalScale);
        //}

        //GameMovement();
        //UpdateState();

        return;
	}

    private void GameMovement()
    {

        float x = Input.GetLeftAxisX();
        float y = Input.GetLeftAxisY();

        gamepadInput = new Vector3(x, y, 0f);

        if (IsMoving())
            inputList.Add(INPUT.IN_MOVE);
        else if (currentState == STATE.MOVE && !IsMoving())
            inputList.Add(INPUT.IN_IDLE);

        if (Input.GetRightTrigger() > 0)
            inputList.Add(INPUT.IN_SHOOT);

        while (inputList.Count > 0)
        {
            INPUT input = inputList[0];
            switch (currentState)
            {
                case STATE.NONE:
                    Debug.Log("CORE ERROR STATE");
                    break;

                case STATE.IDLE:
                    switch (input)
                    {
                        case INPUT.IN_MOVE:
                            currentState = STATE.MOVE;
                            Move();
                            break;

                        case INPUT.IN_DASH:
                            currentState = STATE.DASH;
                            Dash();
                            break;

                        case INPUT.IN_SHOOTING:
                            currentState = STATE.SHOOT;
                            Shoot();
                            break;

                    }
                    break;
                case STATE.MOVE:
                    switch (input)
                    {
                        case INPUT.IN_IDLE:
                            currentState = STATE.IDLE;              
                            break;

                        case INPUT.IN_DASH:
                            currentState = STATE.DASH;
                            Dash();
                            break;

                        case INPUT.IN_SHOOTING:
                            currentState = STATE.SHOOT;
                            Shoot();
                            break;

                        case INPUT.IN_DEAD:
                            break;
                    }
                    break;
                case STATE.SHOOT:
                    switch (input)
                    {
                        case INPUT.IN_SHOOT_END:
                            currentState = STATE.SHOOT;
                            Shoot();
                            break;

                        case INPUT.IN_DEAD:
                            break;
                    }
                    break;

                case STATE.DASH:
                    switch (input)
                    {
                        case INPUT.IN_DASH_END:
                            currentState = STATE.IDLE;
                            break;
                    }
                    break;
            }

        }
    }

    private void UpdateState()
    {
        switch (currentState)
        {
            case STATE.NONE:
                break;
            case STATE.IDLE:
                break;
            case STATE.MOVE:
                RotatePlayer();
                reference.transform.localPosition += reference.GetForward() * movementSpeed * Time.deltaTime;
                break;
            case STATE.DASH:
                
                break;
            case STATE.SHOOT:
                
                break;
            default:
                Debug.Log("NEED TO ADD STATE TO CORE");
                break;
        }
    }

    private void RotatePlayer()
    {
        //Calculate player rotation
        Vector3 aX = new Vector3(gamepadInput.x, 0, -gamepadInput.y - 1);
        Vector3 aY = new Vector3(0, 0, 1);
        aX = Vector3.Normalize(aX);

        if (aX.x >= 0)
        {
            angle = Math.Acos(Vector3.Dot(aX, aY) - 1);
        }
        else if (aX.x < 0)
        {
            angle = -Math.Acos(Vector3.Dot(aX, aY) - 1);
        }

        //Convert angle from world view to orthogonal view
        angle += 0.785398f; //Rotate 45 degrees to the right

        reference.transform.localRotation = Quaternion.RotateAroundAxis(Vector3.up, (float)-angle);
    }

    private void Move()
    {
         // Play Audio and SetAnimation;
    }
    private void Dash()
    {
        // Play Audio and SetAnimation;
    }
    private void Shoot()
    {
        // Play Audio and SetAnimation;
    }

    private bool IsMoving()
    {
     
        return Math.Abs( gamepadInput.x) > 0 || Math.Abs(gamepadInput.y) > 0;
    }
}




