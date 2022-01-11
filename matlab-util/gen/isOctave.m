function flag_octave = isOctave()

flag_octave = exist('OCTAVE_VERSION', 'builtin')~=0;

end

