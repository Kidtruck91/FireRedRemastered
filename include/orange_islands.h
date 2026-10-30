#ifndef GUARD_ORANGE_ISLANDS_H
#define GUARD_ORANGE_ISLANDS_H

// Puts RAM into the Orange Islands challenge state -- the Orange PC and an
// Orange team -- without the flash round trip OrangeChallenge_Begin does.
// Only for the ORANGE_TEST_START build, where there is nothing to preserve.
void OrangeTest_EnterChallenge(void);
void OrangeTestStart(void);

#endif // GUARD_ORANGE_ISLANDS_H
