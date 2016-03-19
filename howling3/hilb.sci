TAP = 31;
FS = 32000; z = poly(0, 'z');
hn = hilb(TAP, 'hm');
hz = [];
for i = TAP:-1:1
	hz = hn(i) +  hz/z;
end;

phase = []; mag = [];
for f = 0: 511
	p = 0;
	for i = 1: TAP
		p = p + hn(i)*exp(2*%pi*f/1024*%i*(i-(TAP+1)/2));
	end;
	phase = [phase atan(imag(p), real(p))];
	mag = [mag abs(p)];
end;
phase(1)=phase(2);
clf;subplot(211);plot(mag);
subplot(212);plot(phase);

xset('window',1);clf;plot(hn);

	s = sin(2*%pi*1000*(1:20000)/FS);
s1 = cos(2*%pi*1000*(1:20000)/FS);

y1 = flts(s, hz);
y2 = s1(24:20000);
y2 = [y2 zeros(1,23)];

clf;plot(y1);plot(y2);

hn=eqfir(25,[0 .005; 0.01 0.015; 0.02 0.03; .07 .5],[0.01 0.2 0.6 1 ],[ .3 .1 .1 1]);
[a f]=frmag(hn, 512);clf;plot(a);
 
