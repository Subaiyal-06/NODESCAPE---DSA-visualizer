#ifndef REPLAYPLAYER_H
#define REPLAYPLAYER_H

// Shows the recorded steps of the last operation, one frame at a time, forward and backward.
void runReplay();

// Use this instead of pauseScreen() at the end of an operation: when replay mode is on and the
// operation recorded steps, it offers the replay, otherwise it just waits for Enter.
void finishOperation();

#endif
