import 'dart:math';

final _random = new Random();

/*
 * Generates a positive random integer uniformly distributed on the range
 * from [min], inclusive, to [max], exclusive.
 */
int getRandomNumber({int min, int max}) => min + _random.nextInt(max - min);
