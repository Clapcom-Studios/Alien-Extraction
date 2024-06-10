using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace YmirEngine
{
    public class Particles
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PlayParticles(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ParticleShoot(object go, object vec, float angleShotgun = 0);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void PlayParticlesTrigger(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void StopParticles(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void RestartParticles(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ParticlesForward(object go, object vec, int emitter, float distance);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void ParticlesSetDirection(object go, object vec, int emitter);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetMaxDistance(object go, float range);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetEmittersPosition(object go, object vec,float emitter = -1);

    }
}
