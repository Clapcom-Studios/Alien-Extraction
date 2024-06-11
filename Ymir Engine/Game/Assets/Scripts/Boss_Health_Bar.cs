using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;


public class Boss_Health_Bar : YmirComponent
{
    public float HP = 500;
    public float initialHP = 500;
    public Vector3 initialScale;

    public GameObject healthBar = null;
    public GameObject boss = null;
    

    //private FaceHuggerBaseScript aux = null;
    //private DroneXenomorphBaseScript aux2 = null;
    //private SpitterBaseScript aux3 = null;
    private QueenXenomorphBaseScript bossScript = null;

    public void Start()
    {

        healthBar = InternalCalls.GetGameObjectByName("Boss_HealtBar");
       


        bossScript = boss.GetComponent<QueenXenomorphBaseScript>();

        SetInitialHP();

        if (healthBar != null)
        {
            UI.SliderSetMax(healthBar, initialHP);
            UI.SliderEdit(healthBar, initialHP);
        }
        
        HP = initialHP;
    }

    public void Update()
    {
        gameObject.transform.localPosition = new Vector3(boss.transform.localPosition.x, gameObject.transform.localPosition.y, boss.transform.localPosition.z);


        gameObject.SetAsBillboard();

        float scaleX = Mathf.Max(0, initialScale.x * (HP / initialHP));
        Vector3 newScale = new Vector3(scaleX, initialScale.y, initialScale.z);
       

        //---------------Xiao: Gurrada Pendiente de Cambiar----------------------------
        if (bossScript != null)
        {
            HP = bossScript.life;
            if (bossScript.GetState() == QueenState.DEAD)
            {
                Destroy();
            }
        }

        
    }

    public void SetInitialHP()
    {
        if (bossScript != null)
        {
            initialHP = bossScript.life;

        }
    }
    public void Destroy()
    {
        InternalCalls.Destroy(gameObject);
    }

}

