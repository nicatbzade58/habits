# habits v0.65
## A CLI habit tracker / day planner

This is a minimalistic CLI app that aims to be able to:
1. Keep a registry of habits.
2. Summarize your daily activities and skills using Tags.
3. Show you how much time you devote to each habit.
4. Set habit goals, and remind them at each use.
5. Set tag goals and promote activities with those tags.

For now you can manage (add,remove,edit) a library of activities, summarize and save them.

### commands:

**new-activity:** Add a new activity to the register.

**list-activities:** List all activities in the register which can be added to today's list.

Note: For now all activities are automatically added to today's list.

**summarize:** List all activities done today and count the tags.

**remove-activity:** Delete an activity from the register.

**save:** Save current list to a file in the current directory.

**help:** Display all possible commands.

### to-do:

**v0.7:** Optimize for multiple sessions: Add delete and edit functions.

**v0.8:** Optimize for multiple sessions: Divide today's activities from habits register.

**v0.9:** Save last week's daily activities and add summarize-week function.
