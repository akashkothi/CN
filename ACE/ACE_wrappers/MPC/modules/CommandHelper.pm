package CommandHelper;

# ************************************************************
# Description   : Base class and factory for all command helpers.
#
#                 The get() method converts the define custom type
#                 provided to uppercase, removes the '_FILES' portion and
#                 adds 'Helper' to the end.  If a module is found matching
#                 that name, it will be used to assist the ProjectCreator
#                 in determining which output files will be generated by
#                 the command given the file name and command options.
#
# Author        : Chad Elliott
# Create Date   : 6/30/2008
# ************************************************************

# ************************************************************
# Pragmas
# ************************************************************

use strict;
use File::Basename;

# ************************************************************
# Data Section
# ************************************************************

my %required;
my %notfound;

# ************************************************************
# Subroutine Section
# ************************************************************

sub get {
  ## Create the helper name
  my $type = uc(shift);
  $type =~ s/_FILES$/Helper/;

  ## Don't search the filesystem again if we didn't find one the first
  ## time we looked.
  return undef if ($notfound{$type});

  ## Return the helper if we've already created one
  return $required{$type} if (defined $required{$type});

  ## Assist users in figuring out why their helper isn't being picked up.
  OutputMessage::debug(undef, "Searching @INC for $type.pm");

  ## If we can find a helper with this name, we will
  ## create a singleton of that type and return it.
  foreach my $inc (@INC) {
    if (-r "$inc/$type.pm") {
      require "$type.pm";
      $required{$type} = $type->new();
      return $required{$type};
    }
  }

  ## We didn't find a helper.  Keep track of that fact and return undef.
  $notfound{$type} = 1;
  return undef;
}

sub new {
  my $class = shift;
  return bless {}, $class;
}

sub get_output {
  ## This method is called with the filename and command options and
  ## returns an array reference containing filenames that will be
  ## generated but can not be described using the normal Define_Custom
  ## syntax.  An optional second return value is a hash reference
  ## describing additional dependencies as {type => {file => [deps]}}.
  ## It's used to tell MPC that 'file' (one of the outputs) belongs to
  ## 'type' and has additional dependencies 'deps'.
  return [];
}

sub get_outputexts {
  ## This method is expected to return an array reference containing the
  ## extensions for files returned by the get_output() method.  They will
  ## be used as regular expressions so regular expression characters
  ## (such as '.', '[', ']', etc.) must be escaped.  This can be done by
  ## calling $self->Parser::escape_regex_special($str);
  return [];
}

sub get_tied {
  ## This method is called with a file name and an array reference of
  ## files.  The first expected return value is an array reference of those
  ## files listed in the passed array reference that are in some way tied
  ## to the file name passed in.  The second is a component name to help
  ## MPC figure out a way to tie the files together.  The result of "tied"
  ## files is that they may be compiled after the file name passed in.
  return [], undef;
}

sub set_creator {
  my($self, $creator) = @_;
  $self->{'creator'} = $creator;
}

1;
