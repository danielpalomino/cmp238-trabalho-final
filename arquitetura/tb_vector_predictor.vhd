
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE ieee.numeric_std.ALL;
 
ENTITY tb_vector_predictor IS
END tb_vector_predictor;
 
ARCHITECTURE behavior OF tb_vector_predictor IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT vector_predictor
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         start : IN  std_logic;
         block_type : IN  std_logic_vector(1 downto 0);
         mv_in : IN  std_logic_vector(0 to 1);
         mv_out : OUT  std_logic_vector(0 to 1)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal start : std_logic := '0';
   signal block_type : std_logic_vector(1 downto 0) := (others => '0');
   signal mv_in : std_logic_vector(0 to 1) := (others => '0');

 	--Outputs
   signal mv_out : std_logic_vector(0 to 1);

   -- Clock period definitions
   constant clk_period : time := 20ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: vector_predictor PORT MAP (
          clk => clk,
          reset => reset,
          start => start,
          block_type => block_type,
          mv_in => mv_in,
          mv_out => mv_out
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100ms.
		reset <= '1';
      wait for 20s;
		reset <='0';
		start <='1';

      wait for clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
