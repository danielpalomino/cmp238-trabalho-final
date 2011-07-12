LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY work;
USE work.VECTOR_LIBRARY.all;

ENTITY data_path IS
	PORT (		
	);
END data_path;

ARCHITECTURE comportamento OF data_path IS

	SIGNAL x_a, x_b, x_c, x_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
	SIGNAL x_a, x_b, x_c, x_s : STD_LOGIC_VECTOR(7 DOWNTO 0);

BEGIN

	MEDIANA_X: mediana PORT MAP (
		a => x_a,
		b => x_b,
		c => x_c,
		s => x_s
	);

	MEDIANA_Y: mediana PORT MAP (
		a => y_a,
		b => y_b,
		c => y_c,
		s => y_s
	);

END comportamento;
