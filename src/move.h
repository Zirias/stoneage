#ifndef STONEAGE_MOVE_H
#define STONEAGE_MOVE_H

typedef double[7][2] *Trajectory;
typedef int[7][2] *PixelTrajectory;

enum TrajectoryType
{
    TR_Linear;
};

CLASS(Move)
{
    INHERIT(Object);
}

#endif
