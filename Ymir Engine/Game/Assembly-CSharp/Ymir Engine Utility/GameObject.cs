using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace YmirEngine
{
    /// <summary>
    /// Represents a game object in the game engine.
    /// </summary>
    public sealed class GameObject
    {
        /// <summary>
        /// The name of the game object.
        /// </summary>
        public string name;

        /// <summary>
        /// Pointer to the game object.
        /// </summary>
        public UIntPtr pointer;

        /// <summary>
        /// The transform component of the game object.
        /// </summary>
        public Transform transform;

        /// <summary>
        /// Default constructor for the game object.
        /// </summary>
        public GameObject()
        {
            name = "Empty";
            pointer = UIntPtr.Zero;
        }

        /// <summary>
        /// Constructor for the game object with parameters.
        /// </summary>
        /// <param name="_name">The name of the game object.</param>
        /// <param name="ptr">Pointer to the game object.</param>
        /// <param name="transPTR">Pointer to the transform component.</param>
        public GameObject(string _name, UIntPtr ptr, UIntPtr transPTR)
        {
            name = _name;
            pointer = ptr;
            transform = new Transform();
            transform.pointer = transPTR;
        }

        /// <summary>
        /// Gets the component of type T.
        /// </summary>
        /// <typeparam name="T">Type of component.</typeparam>
        /// <returns>The component of type T if found, otherwise null.</returns>
        public T GetComponent<T>() //where T : YmirComponent
        {
            ComponentType retValue = ComponentType.SCRIPT;
            if (YmirComponent.componentTable.ContainsKey(typeof(T)))
            {
                retValue = YmirComponent.componentTable[typeof(T)];
            }

            return TryGetComponent<T>(typeof(T).ToString(), (int)retValue);
        }

        /// <summary>
        /// Tries to get the component of type T.
        /// </summary>
        /// <typeparam name="T">Type of component.</typeparam>
        /// <param name="type">Type of the component as a string.</param>
        /// <param name="inputType">Type of the component as an integer.</param>
        /// <returns>The component of type T if found, otherwise null.</returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal T TryGetComponent<T>(string type, int inputType = 0);

        /// <summary>
        /// Gets the name of the game object.
        /// </summary>
        public extern string Name
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        /// <summary>
        /// Gets the parent of the game object.
        /// </summary>
        public extern GameObject parent
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;
        }

        /// <summary>
        /// Gets or sets the tag of the game object.
        /// </summary>
        public extern string Tag
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            get;

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            set;
        }

        /// <summary>
        /// Gets the hash code of the game object.
        /// </summary>
        /// <returns>The hash code of the game object.</returns>
        int GetHash()
        {
            return this.GetHashCode();
        }

        /// <summary>
        /// Sets the activity state of the game object.
        /// </summary>
        /// <param name="active">The activity state to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool active);

        // The following methods are commented out to fix duplicated methods

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern Vector3 GetForward();

        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern Vector3 GetRight();

        /// <summary>
        /// Sets the velocity of the game object.
        /// </summary>
        /// <param name="velocity">The velocity to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetVelocity(Vector3 velocity);

        /// <summary>
        /// Sets the impulse of the game object.
        /// </summary>
        /// <param name="impulse">The impulse to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetImpulse(Vector3 impulse);

        /// <summary>
        /// Adds a component to the game object.
        /// </summary>
        /// <param name="componentType">The type of the component to add.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void AddComponent(int componentType);

        /// <summary>
        /// Sets the rotation of the game object.
        /// </summary>
        /// <param name="rot">The rotation to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetRotation(Quaternion rot);

        /// <summary>
        /// Sets the position of the game object.
        /// </summary>
        /// <param name="position">The position to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetPosition(Vector3 position);

        /// <summary>
        /// Sets the size of the collider *Not the game object scale*.
        /// </summary>
        /// <param name="size">The size to set.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetColliderSize(Vector3 size);

        /// <summary>
        /// Get size of the collider.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern Vector3 GetColliderSize();
        //public extern Vector3 GetColliderSize
        //{
        //    [MethodImplAttribute(MethodImplOptions.InternalCall)]
        //    get;
        //}

        /// <summary>
        /// Clear all forces of the game object's collider.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void ClearForces();


        /// <summary>
        /// Find the taged children inside a specific GO
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern GameObject GetChildrenByTag(string tag);

        /// <summary>
        /// Find the taged children inside a specific GO
        /// </summary>
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public extern bool Raycast(object origin, object direction, float rayLenght, object hitPoints, object hits);

        /// <summary>
        /// Returns the gameObject that has collided with the ray
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern GameObject RaycastHit(object origin, object direction, float rayLenght);

        /// <summary>
        /// Check if a gameObject has collided with a raycast
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool RaycastTest(object origin, object direction, float rayLenght);

        /// <summary>
        /// Returns the distance from a gameObject that the raycast has collided with
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern float RaycastLenght(object origin, object direction, float rayLenght);

        /// <summary>
        /// Set collider active/inactive
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetColliderActive(bool isActive);

        /// Make the GO look at the camera
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetAsBillboard();


        /// <summary>
        /// Returns true if the GameObject is active, and false otherwise
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern bool IsActive();

        /// <summary>
        /// Sets the scale of the gameobject
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern void SetScale(Vector3 vec);
    }
}
