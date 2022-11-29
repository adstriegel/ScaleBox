
# From http://stackoverflow.com/questions/4481005/get-wireless-ssid-through-shell-script-on-mac-os-x

open(WIFI_INFO,"/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I |") || die "Failed: $!\n";

while(<WIFI_INFO>)
{
    print WIFI_INFO;
}

