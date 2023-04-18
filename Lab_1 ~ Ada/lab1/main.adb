-- Tyler Crabb
-- Carter Nettesheim

with Ada.Text_IO;
use  Ada.Text_IO;

with Text_Io;
use  Text_Io;

with Ada.Calendar;
use  Ada.Calendar;

procedure main is

   F1_Start, F2_Start, F3_Start: Duration;
   vTime, vTime2, F1_Curr, F2_Curr, F3_Curr, Before, After: Duration;

   package DIO is new Text_Io.Fixed_Io(Duration); --To print Duration variables you can instantiate the generic
						  --package Text_Io.Fixed_Io with a duration type:
						  --"package DIO is new Text_Io.Fixed_Io(Duration);"
						  --The DIO package will then export, among other things,
						  --the procedure DIO.Put(D:Duration, Fore:Field, Aft:Field)
						  --to print variable D of type Duration. See an example
						  --on how to use this below.

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
	 DIO.Put(Currtime + (FinishF3 - StartF3)); --Needed since time starts at 0 and FinishF1 and StartF1 are not virtual times
      end if;
   end F3;
------------------------start of part 4 procedure----------------------------------------------------------
begin
   vTime := 0.0;
   Before := Ada.Calendar.Seconds(Ada.Calendar.Clock); -- relative time (type duration) from program start

   --Main loop
   loop
      After := Ada.Calendar.Seconds(Ada.Calendar.Clock);


     if After - Before >= 1.000000000 then

         vTime := vTime + (After - Before); --Needed since time starts at 0

         F1_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F1
         F1(Currtime => vTime, StartF1 => 0.0, FinishF1 => 0.0); --Call F1 procedure. Initialize F1
         loop  ---------------------F1 is executed every second with an execution time of 0.3s------------

            --Get current time
            F1_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

            exit when  F1_Curr - F1_Start >= 0.3000; --Assuming F1 takes 0.3 seconds

         end loop; --F1 ends
    --After F1 finishes executing, call the F1 procedure again to obtain the finish time
         F1(Currtime => vTime, StartF1 => F1_Start, FinishF1 => F1_Curr);
         F2_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F2

 ----------------------------------------------------------------------F2, which executes for 0.15s, starts when F1 terminates--------------------------------------------------------------------
	vTime2 := vTime + F1_Curr - F1_Start;
    vTime :=  + (F1_Curr - F1_Start); --Changes relative start time for F2

    F2(Currtime => vTime2, StartF2 => 0.0, FinishF2 => 0.0); --Initialize F2
    loop --F2 starts

	    --Get current time
	    F2_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

	    exit when  F2_Curr - F2_Start >= 0.15000; --Assuming F2 takes 0.15 seconds

	 end loop; --F2 ends
	 --After F2 finishes executing, call the F2 procedure again to obtain the finish time
         F2(Currtime => vTime2, StartF2 => F2_Start, FinishF2 => F2_Curr);

 -----------------------------------------------------------------------F3 starts executing 0.5s after F1 starts and its execution time is 0.2s----------------------------------------------------
    F3_Start := Ada.Calendar.Seconds(Ada.Calendar.Clock); --Get start time of F3
    --vTime := Program_Start + (F1_Curr - F1_Start); --Changes relative start time for F3

    F3(Currtime => vTime, StartF3 => 0.0, FinishF3 => 0.0); --Initialize F2
    loop --F3 starts

	    --Get current time
	    F3_Curr := Ada.Calendar.Seconds(Ada.Calendar.Clock);

	    exit when  F3_Curr - F3_Start >= 0.20000; --Assuming F3 takes 0.2 seconds

	 end loop; --F3 ends
	 --After F3 finishes executing, call the F3 procedure again to obtain the finish time
      F3(Currtime => vTime, StartF3 => F3_Start, FinishF3 => F3_Curr);
     end if;





   end loop;
  end main;
