my $url ='http://129.74.161.212/wc-perl/mac.html';

# Simple script
use LWP::Simple;

my $count=0;
my $length_dl=0;

while(1)
{
    my $content = get $url;
    $length_dl = length $content;
    print "$count: Downloaded $length_dl bytes successfully .... sleeping again\n";
    $count += 1;
    sleep 15;
}

