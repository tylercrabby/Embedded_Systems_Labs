-- Tyler Crabb
-- Carter Nettesheim

with TEXT_IO;
use TEXT_IO;

with Ada.TEXT_IO;
use Ada.TEXT_IO;

with Ada.Calendar;
use Ada.Calendar;

with Ada.Numerics;
use Ada.Numerics;
with Ada.Numerics.Discrete_Random;

procedure part_6  is

package DIO is new Text_Io.Fixed_Io(Duration); --allows printing of Duration variables
   
----------- Produce a FIFO buffer to store Integer values in. -----------

-- Declare the BigBadBuffer. It will push values into the buffer and pop values off the buffer in FIFO.
task BigBadBuffer is
	entry push(newValue: in Integer);
	entry pull(poppedValue: out Integer);
	entry setOneHundredReached;
end BigBadBuffer;

--	The buffer is used to store Integers in a FIFO queue.
task body BigBadBuffer is
  bufferArray : array (0 .. 9) of Integer; -- Give us a buffer of 10 Integers to store values in.
  position : Integer := 0; -- Current position of the next value in the buffer.
  head, tail, current : Integer := 0; -- Initialize each position to 0 to begin with.
  oneHundredReached : Boolean := False;	-- This boolean is used to flag when the sum of the dequeued Integers is 100 or more.
      
   begin
      -- Initialize the buffer to hold all -1's.
      for position in bufferArray'Range loop
         bufferArray(position) := -1;
      end loop;
      
      -- Scan push and pull to see if the buffer needs to add or remove values.
      loop
         select
            -- If there is room on the buffer, push a new value into the buffer.
            when position < current + 1 => accept push(newValue : in Integer) do
                  bufferArray(head) := newValue; -- Assign the new Integer to the buffer.
				if head > bufferArray'length then	 -- We need this to cover overflow of the buffer length.
					head := 0;
				else
					head := head + 1;
				end if;
				current := current + 1;
			end push;
         or
            -- This pulls the oldest value off the buffer. 
            when current > 0 => accept pull(poppedValue : out Integer) do
               poppedValue := bufferArray(tail); -- Give me the next value off the buffer.
               bufferArray(tail) := -1; -- Reset the reopened position in the buffer to -1.
                  
			  if tail > (bufferArray'length - 1) then -- If the tail steps off the end of the buffer ->
               tail := 0; -- Wrap the tail around to the beginning of the buffer.
            else
			     tail := tail + 1; -- Step the tail to the next position in the buffer.
            end if;
                  
               current := current - 1; -- Move current to the previous value.
                  
			 end pull;
			or 
            -- Check to see if the sum of buffered values exceeds 100.
            accept setOneHundredReached do
				oneHundredReached := True; -- Set the flag for reaching 100 if it is time to terminate.
			end setOneHundredReached;
         end select;
         
	exit when oneHundredReached = True;
	end loop;
end BigBadBuffer;

----------- Generates new Integers to be added to the buffer. -----------

-- Declare the generator
task integerGenerator is  
	entry setOneHundredReached; 
end integerGenerator;

-- Produces a random Integer from 0 to 25 to be queued.
task body integerGenerator is
   subtype Num_Gen is Integer range 0 .. 25;
   -- Produce some random Integer values within 25   
	package Random_Gen is new Ada.Numerics.Discrete_Random(Num_Gen);
	use Random_Gen;
	seed: Random_Gen.Generator;
	
   -- Give another random Integer to delay the generator by in hundred milliseconds.
	subtype GeneratorDelay is Integer range 0 .. 9;
	package Random_Gen_delay is new Ada.Numerics.Discrete_Random(GeneratorDelay);
	use Random_Gen_delay;
	randDelay: Random_Gen_delay.Generator;

   -- Flag to stop generating new Integers.
	oneHundredReached : Boolean := False;
	Input : Integer;

   -- Setup the delay used in generating new Integers.
	begin
      loop
         reset(seed); -- Reroll the random number.
         reset(randDelay); -- Reroll the random delay count.
         Input := Random(seed); -- Assign the new random Integer.
         
         delay Duration(float(Random(randDelay)) / 10.0); -- Wait a while.
         
         BigBadBuffer.Push(Input); -- Shove the new Integer onto the buffer, if there is room.
         Put(Integer'Image(Input)); -- Print the new Integer.
         Put_Line(" was added to the buffer.");
         Put_Line("");
         
         select
            accept setOneHundredReached  do
					oneHundredReached := True;
				end setOneHundredReached;
			or
          -- Delay for the randomly set amount of seconds.
              delay (float(Random(randDelay)));
			end select;
			exit when oneHundredReached = True ;
		end loop;
end integerGenerator;

----------- Consumes Integers from the buffer and keeps a running total. -----------

task integersFromBuffer is
	entry oneHundredReached;
end integersFromBuffer;

task body integersFromBuffer is
   -- Get a random Integer to delay the generator by in hundred milliseconds.
	subtype GeneratorDelay is Integer range 0 .. 9;
	package Random_Gen_delay is new Ada.Numerics.Discrete_Random(GeneratorDelay);
	use Random_Gen_delay;
	randDelay: Random_Gen_delay.Generator;

   -- Initialize the flag, the consumed Integer, and the sum total.
	oneHundredReached : Boolean := False ;
	Output : Integer := 0; 
	Total : Integer := 0; 

   -- Run the consuming task.
	begin
		loop
         reset(randDelay); -- Seed a new random value
         
         delay Duration(float(Random(randDelay)) / 10.0) + 0.1;
         
         -- What value was pulled off the buffer?
         BigBadBuffer.pull(Output);
         Put(Integer'Image(Output));
         Put_Line(" was pulled off the buffer.");
		
         -- Sum the new value to the total and report.
         Total := Total + Output;
         Put("The running Integer total is: ");
         Put_Line(Integer'Image(Total));
         
         -- Is the total 100 or more?
         if Total >= 100 then
            Put("The total reached ");
            Put(Integer'Image(Total));
            Put("!");
            
            -- Stop the other tasks from running.
            BigBadBuffer.setOneHundredReached;
            integerGenerator.setOneHundredReached;
            exit;
			end if;
         end loop;
end integersFromBuffer;		

-- This procedure just starts the tasks at the same time.
begin

BigBadBuffer;
   
   -- Give the tasks some time to work their magic.
   delay (2.0);
   
end part_6;
