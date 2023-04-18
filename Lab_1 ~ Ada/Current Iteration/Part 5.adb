with Ada.Text_IO;
use  Ada.Text_IO;

with Text_Io;
use  Text_Io;

with Ada.Calendar;
use  Ada.Calendar;

with Ada.Numerics;
use  Ada.Numerics;

with Ada.Numerics.Discrete_Random;

procedure hello is
   --Declarations
   Current, Before, After: Time;
   F1_Start, F2_Start, F3_Start: Duration;
   vTime, F1_Curr, F2_Curr, F3_Curr: Duration;
   vTime2: Duration; -- Delay incurred by running F1 / Jitter control
   vTime3: Duration; -- Delay incurred by running F1 / Jitter control
                                          
   rand_duration: Duration;
   
   package DIO is new Text_Io.Fixed_Io(Duration); --To print Duration variables you can instantiate the generic
                                                 --package Text_Io.Fixed_Io with a duration type:
                                                  --"package DIO is new Text_Io.Fixed_Io(Duration);"
                                                  --The DIO package will then export, among other things,
                                                  --the procedure DIO.Put(D:Duration, Fore:Field, Aft:Field)
                                                  --to print variable D of type Duration. See an example
                                                  --on how to use this below.

   --An integer between 0 and 10 used in determine WHEN F3 exceeds its deadline
   --subtype Num_Gen is Integer range 0 .. 4;
   --package Random_Gen is new Ada.Numerics.Discrete_Random(Num_Gen);
   --use Random_Gen;
   seed: Generator;
      --A float between 0 and 1 used in determining by HOW MUCH F3 exceeds deadline
   rand_float: float;

   --Declare F1, which prints out a message when it starts and stops executing
   procedure F1(Currtime: Duration; StartF1: Duration; FinishF1: Duration) is
   begin
      if StartF1 = 0.0 and then FinishF1 = 0.0 then
         Put_Line(""); --Add a new line
         Put_Line("F1 has started executing. The time is now:");
         DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F1 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF1 - StartF1)); --Needed since time starts at 0 and FinishF1 and StartF1 are not virtual times
      end if;
   end F1;

   --F2, which executes for 0.15s, starts when F1 terminates
     procedure F2(Currtime: Duration; StartF2: Duration; FinishF2: Duration) is
begin
      if StartF2 = 0.0 and then FinishF2 = 0.0 then
         Put_Line(""); --Add a new line
         Put_Line("F2 has started executing. The time is now:");
         DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F2 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF2 - StartF2)); --Needed since time starts at 0 and FinishF2 and StartF2 are not virtual times
      end if;
     end F2;

--F3 starts executing 0.5s after F1 starts and its execution time is 0.2s
procedure F3(Currtime: Duration; StartF3: Duration; FinishF3: Duration) is
   begin
      if StartF3 = 0.0 and then FinishF3 = 0.0 then
            Put_Line(""); --Add a new line
            Put_Line("F3 has started executing. The time is now:");
            DIO.Put(Currtime);
      else
         Put_Line("");
         Put_Line("F3 has finished executing. The time is now:");
         DIO.Put(Currtime + (FinishF3 - StartF3)); --Needed since time starts at 0 and FinishF3 and StartF3 are not virtual times
      end if;
end F3;

--Define a Watchdog task to monitor F3 exectuion
   task type watchdog is
   	entry start;
   	entry finish;
   end watchdog;

   task body watchdog is
   	execution_time, watchdog_start, watchdog_finish: Duration;
   begin
      loop
         --Prepare for service
         select
            accept start do
watchdog_start := Ada.Calendar.Seconds(Ada.Calendar.Clock);
            end start;
         or
            accept finish  do
               watchdog_finish := Ada.Calendar.Seconds(Ada.Calendar.Clock);
            end finish;
            execution_time := watchdog_finish - watchdog_start;
         end select;
         if ((execution_time) > 0.5) then --Flags when F3 misses a deadline
                  --Display warning message
                  Put_Line("");
                  Put_Line("WARNING: F3 has missed it's deadline by: ");
                  Put_Line(Duration'Image(execution_time - 0.2000));
                  Put_Line(" seconds.");
                  Put_Line("");       
                  Put_Line("It's execution time was: ");
                  Put_Line(Duration'Image(execution_time));
                  Put_Line("");
                  --Resynchronize cyclic scheduler
                  loop
                     Current := Ada.Calendar.Clock;
                     exit when (Current - Before) >= 1.5000;
                  end loop;
               else
                  --Do nothing and continue to monitor
                  null;
         end if;
      end loop;
   end watchdog;

   F3_watchdog: watchdog;  --Instantiate a watchdog task for F3



-------------------------------------------------------------------------------------------------------------------------------
--                                              start of part 5 procedure
-------------------------------------------------------------------------------------------------------------------------------
begin
   Reset(seed); --Seed the random number generator

   vTime := 0.0;
   Before := Ada.Calendar.Clock; -- relative time (type duration) from program start

   --Main loop
   loop
    rand_float :=  Random(seed); --Get the random number and save as rand_float
    rand_duration := Duration(rand_float); --Type cast the random float to a duration
    After := Ada.Calendar.Clock;

    if After - Before >= 1.000000000 then

-----------------------------------------------------------------------F1 Starts and executes every second with an execution time of 0.3s-------------------------------------------------------------------------------

        vTime := vTime + (After - Before); --Needed since time starts at 0

        F1_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F1

         F1(Currtime => vTime, StartF1 => 0.0, FinishF1 => 0.0); --Call F1 procedure. Initialize F1
         loop

            --Get current time
            F1_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            exit when  F1_Curr - F1_Start >= 0.2000; --Assuming F1 takes 0.3 seconds

         end loop; --F1 ends
    --After F1 finishes executing, call the F1 procedure again to obtain the finish time
        F1(Currtime => vTime, StartF1 => F1_Start, FinishF1 => F1_Curr);
        F2_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F2

        Before := After; -- Resetting Before so that F1 can run every second

 -----------------------------------------------------------------------F2 starts when F1 terminates and executes for 0.15s, --------------------------------------------------------------------
    vTime2 := vTime + F1_Curr - F1_Start; -- How long did F1 take to run?

    F2(Currtime => vTime2, StartF2 => 0.0, FinishF2 => 0.0); --Initialize F2
    loop --F2 starts

            --Get current time
            F2_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            exit when  F2_Curr - F2_Start >= 0.15000; --Assuming F2 takes 0.15 seconds

         end loop; --F2 ends
         --After F2 finishes executing, call the F2 procedure again to obtain the finish time
         F2(Currtime => vTime2, StartF2 => F2_Start, FinishF2 => F2_Curr);

 -----------------------------------------------------------------------F3 starts 0.5s after F1 starts and executes for 0.2s----------------------------------------------------

    vTime3 := vTime + 0.5; -- Start running 0.5 seconds after F1 does.


    F3_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F3


    F3(Currtime => vTime3, StartF3 => 0.0, FinishF3 => 0.0); --Initialize F3
    F3_watchdog.start; --Watchdog catches start time

    loop --F3 starts
            --rand:= Random(seed); -- Assign random numbers for both WHEN (rand_int) and by HOW MUCH (rand_float) F3 will exceed its deadline
            --rand_duration := duration(rand);
            
            --Get current time
            F3_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            --if(rand_int < 4) then --Random exceeded computation time for F3
               exit when F3_Curr - F3_Start >= rand_duration;
            --end if;

            --exit when  F3_Curr - F3_Start >= 0.20000; --Assuming F3 takes 0.2 seconds
--Inequality is greater than or equal to because F3 is allowed to continue execution even though it missed a deadline.
      end loop; --F3 ends

    --After F3 finishes executing, call the F3 procedure again to obtain the finish time.
    F3(Currtime => vTime3, StartF3 => F3_Start, FinishF3 => F3_Curr);
    F3_watchdog.finish; --Watchdog catches finish time

     end if; -- Encapsulates the whole cyclic scheduler

  end loop; --End of Main loop

end hello;