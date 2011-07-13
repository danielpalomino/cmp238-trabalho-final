LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_SIGNED.ALL;

ENTITY mediana IS
	PORT (
		a, b, c : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		s : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
END mediana;

ARCHITECTURE comportamento OF mediana IS

	SIGNAL comp_ab, comp_bc, comp_ca : STD_LOGIC;
	SIGNAL r_comp : STD_LOGIC_VECTOR(2 DOWNTO 0);

BEGIN

	r_comp(2) <= comp_ab;
	r_comp(1) <= comp_bc;
	r_comp(0) <= comp_ca;
	
	PROCESS(a, b, c)
	BEGIN
	  IF a < b THEN
	    comp_ab <= '1';
	  ELSE
	    comp_ab <= '0';
	  END IF;
	  IF b < c THEN
	    comp_bc <= '1';
	  ELSE
	    comp_bc <= '0';
	  END IF;	  
	  IF c < a THEN
	    comp_ca <= '1';
	  ELSE
	    comp_ca <= '0';
	  END IF;
	END PROCESS;
	
	-- B < A < C : 010 (SAIDA A)
	-- C < A < B : 101 (SAIDA A)
	-- A < B < C : 110 (SAIDA B)
	-- C < B < A : 001 (SAIDA B)
	-- A < C < B : 100 (SAIDA C)
	-- B < C < A : 011 (SAIDA C)

	WITH r_comp SELECT
		s <= a WHEN "010",
			 a WHEN "101",
			 b WHEN "110",
			 b WHEN "001",
			 c WHEN "100",
			 c WHEN OTHERS;

END comportamento;
