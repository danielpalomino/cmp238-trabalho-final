LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE ieee.numeric_std.ALL;
 
ENTITY tb_control IS
END tb_control;
 
ARCHITECTURE behavior OF tb_control IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT control
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         start : IN  std_logic;
         block_type : IN  std_logic_vector(1 downto 0);
         we : OUT  std_logic;
         enable_reg_A : OUT  std_logic;
         enable_reg_B : OUT  std_logic;
         enable_reg_C : OUT  std_logic;
         enable_reg0 : OUT  std_logic;
         enable_reg1 : OUT  std_logic;
         enable_reg2 : OUT  std_logic;
         enable_reg3 : OUT  std_logic;
         mux_A : OUT  std_logic_vector(1 downto 0);
         mux_B : OUT  std_logic_vector(1 downto 0);
         mux_C : OUT  std_logic_vector(1 downto 0);
         mux_write_memory : OUT  std_logic_vector(1 downto 0);
         memory_address : OUT  std_logic_vector(7 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal start : std_logic := '0';
   signal block_type : std_logic_vector(1 downto 0) := (others => '0');

 	--Outputs
   signal we : std_logic;
   signal enable_reg_A : std_logic;
   signal enable_reg_B : std_logic;
   signal enable_reg_C : std_logic;
   signal enable_reg0 : std_logic;
   signal enable_reg1 : std_logic;
   signal enable_reg2 : std_logic;
   signal enable_reg3 : std_logic;
   signal mux_A : std_logic_vector(1 downto 0);
   signal mux_B : std_logic_vector(1 downto 0);
   signal mux_C : std_logic_vector(1 downto 0);
   signal mux_write_memory : std_logic_vector(1 downto 0);
   signal memory_address : std_logic_vector(7 downto 0);

   -- Clock period definitions
   constant clk_period : time := 20ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: control PORT MAP (
          clk => clk,
          reset => reset,
          start => start,
          block_type => block_type,
          we => we,
          enable_reg_A => enable_reg_A,
          enable_reg_B => enable_reg_B,
          enable_reg_C => enable_reg_C,
          enable_reg0 => enable_reg0,
          enable_reg1 => enable_reg1,
          enable_reg2 => enable_reg2,
          enable_reg3 => enable_reg3,
          mux_A => mux_A,
          mux_B => mux_B,
          mux_C => mux_C,
          mux_write_memory => mux_write_memory,
          memory_address => memory_address
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
      wait for 20ns;
		reset <= '0';

      start <= '1' ;
		wait for 20ns;
		start <= '0';
		
		wait for 240ns;
		block_type <= "01";
		start <= '1' ;
		wait for 20ns;
		start <= '0';
		wait for 240ns;
		block_type <= "10";
		start <= '1';
		wait for 20ns;
		start <= '0';
		wait for 240ns;
    block_type <= "11";
    start <= '1';
    wait for 20ns;
    start <= '0';
      wait;
		
   end process;

END;
