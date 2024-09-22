import {Component, OnInit} from '@angular/core';
import {ApiService} from "../../services/api.service";
import {User} from "../../../User";
import {faRefresh} from "@fortawesome/free-solid-svg-icons";

@Component({
  selector: 'app-listuser',
  templateUrl: './list-user.component.html',
  styleUrls: ['./list-user.component.css']
})
export class ListUserComponent implements OnInit{
  users: User[];
  showUserInfo: boolean = false;
  sortByField= "";
  sortDirection = true;
  checkboxValues: boolean[] = [true, true, true, true, true, true, true, true];
  totalPages: number;
  currentPage: number = 0;
  pageLimit: number;

  constructor(private apiService: ApiService) {
  }

  ngOnInit() {
    this.apiService.getUsers("", 1, "", -1).subscribe((data: User[]) => {
      this.users = data["users"];
      this.totalPages = data["totalPages"];
      }
    );
  }

  onRefresh(){
    if(this.pageLimit){
      this.apiService.userPageLimit = this.pageLimit;
    }
    this.ngOnInit();
    this.sortByField = "";
  }

  sortBy(field){
    if(field === "birthdate"){
      field = "dateOfBirth";
    }
    if(field === this.sortByField){
      console.log("HERE")
      this.sortDirection = !this.sortDirection;
    }
    else{
      this.sortDirection = true;
    }
    this.sortByField = field;
    this.goPage(0);
  }

  onSwitchChange(event: Event) {
    this.showUserInfo = (event.target as HTMLInputElement).checked;
    // You can now use this.showUserInfo to determine whether to show user information or not
    if (this.showUserInfo) {
      console.log('User information should be shown.');
    } else {
      console.log('User information should be hidden.');
    }
  }

  onCheckBox(event: Event, index){
    this.checkboxValues[index] = (event.target as HTMLInputElement).checked;
    console.log(this.checkboxValues, index);
  }

  deleteUser(username: String, index){
    this.apiService.delete(username).subscribe(value => {
      this.users.splice(index, 1)
      alert(username +" deleted.")
      }
    );
  }

  goPage(index){
    this.currentPage = index;
    this.apiService.getUsers("", index + 1, this.sortByField, this.sortDirection ? 1 : -1).subscribe((data) => {
      this.users = data["users"];
      console.log(this.sortDirection)
      this.totalPages = data["totalPages"];
    });

  }

  goPrev(){
    if(this.currentPage === 0){
      return;
    }
    else{
      this.goPage(this.currentPage - 1)
    }
  }

  goNext(){
    if(this.currentPage === (this.totalPages - 1)){
      return;
    }
    else{
      this.goPage(this.currentPage + 1);
    }
  }


  protected readonly faRefresh = faRefresh;
}
