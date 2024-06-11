using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using YmirEngine;

//public struct Cinematic
//{
//    public string _name;
//    public List<TravelKey> _keys;
//    public bool _isPlaying;

//    public  Cinematic(string name)
//    {
//        _name = name;
//        _keys = new List<TravelKey>();
//        _isPlaying = false;
//    }

//    public void PlayCinematic()
//    {
//        _isPlaying = true;

//        for (int i = 0; i < _keys.Count(); i++)
//        {

//        }
//    }
//}
//public struct TravelKey
//{
//    public Vector3 _position;
//    public Vector3 _rotation;

//    public float _travelTime;

//    public TravelKey(Vector3 position, Vector3 rotation, float time)
//    {
//        _position = position;
//        _rotation = rotation;
//        _travelTime = time;
//    }
//}
public class Camera : YmirComponent
{
    public enum FOLLOW : int
    {
        NULL = -1,

        PLAYER,
        CINEMATIC
    }

    // Offset between camera/player
    public Vector3 offset = new Vector3(-58, 111, 63);
    public Vector3 rotationOffset = new Vector3(120, 25, -142);
    //public Vector3 rotationOffset = new Vector3(120, 270, 0);

    // Player
    private GameObject playerObject;
    private Player player;

    // Timers
    private float idleTimer;
    private float delayTimer;

    private float idleTimerMax = 2.0f;
    private float delayTimerMax = 5.6f;

    private bool delay;

    public const float constDelay = 0.3f;

    public FOLLOW followState = FOLLOW.PLAYER;

    //public List<Cinematic> cinematics;

    public void Start()
    {
        // Cinematics

        //Cinematic testCinematic = new Cinematic("TestCinematic");
        //testCinematic._keys.Add(new TravelKey(new Vector3(0f, 0f, 0f), new Vector3(0f, 0f, 0f), 3f));

        //cinematics.Add(testCinematic);

        //// ----------

        ////gameObject.transform.localRotation = Quaternion.Euler(rotationOffset.x, rotationOffset.y, rotationOffset.z);
        //gameObject.SetRotation(Quaternion.Euler(rotationOffset.x, rotationOffset.y, rotationOffset.z));

        //Debug.Log("Local Rotation: " + gameObject.transform.localRotation.ToString());
        //Debug.Log("Rotation Offset: " + Quaternion.Euler(rotationOffset.x, rotationOffset.y, rotationOffset.z).ToString());

        //idleTimer = 0.0f;
        //delayTimer = 0.0f;

        //delay = false;

        //playerObject = InternalCalls.GetGameObjectByName("Player");
        //if (playerObject != null)
        //{
        //    player = playerObject.GetComponent<Player>();
        //}

        Audio.PlayEmbedAudio(gameObject);
    }

    public void Update()
    {



        // Follow Player (If unrelated stuff gets added, write above this comment)

        //Vector3 newpos = playerObject.transform.localPosition + offset;

        //float distance = Vector3.Distance(gameObject.transform.localPosition, newpos);

        //if (player.currentState == Player.STATE.IDLE)
        //{
        //    idleTimer += Time.deltaTime;

        //    if (idleTimer > idleTimerMax)
        //    {

        //        delay = true;
        //        followState = FOLLOW.CINEMATIC;
        //    }
        //}
        //else
        //{
        //    idleTimer = 0.0f;

        //}

        //if (delay)
        //{
        //    delayTimer += Time.deltaTime;

        //    if (delayTimer < delayTimerMax)
        //    {
        //        return;
        //    }
        //    else
        //    {
        //        delay = false;
        //        delayTimer = 0.0f;
        //        followState = FOLLOW.PLAYER;
        //    }
        //}

        //switch (followState)
        //{
        //    case FOLLOW.PLAYER:

        //        //gameObject.transform.localPosition = (Vector3.Lerp(gameObject.transform.localPosition, newpos, Time.deltaTime * distance * constDelay));

        //        break;
        //    case FOLLOW.CINEMATIC:

        //        //gameObject.transform.localPosition = (Vector3.Lerp(gameObject.transform.localPosition, new Vector3(0f, 0f, 0f), Time.deltaTime * delayTimerMax));

        //        break;
        //}
    }
}

